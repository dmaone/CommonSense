/*
 *
 * Copyright (C) 2016-2017 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "PSoC_USB.h"

#include <project.h>
#include <stdio.h>

#include "gpio.h"
#include "globals.h"
#include "scan.h"


// for xprintf - stdio + stdarg
#include <stdarg.h>

#define USB_STATUS_CONNECTED 0
#define USB_STATUS_DISCONNECTED 1
uint8_t usb_status = USB_STATUS_DISCONNECTED;
uint8_t wakeup_enabled = 0;

typedef struct {
  uint8_t EP;
  uint8_t len;
  uint8_t data[64];
} UsbPdu_t;

#define USB_BUFFER_END 15
#define USB_BUFFER_NEXT(X) ((X + 1) & USB_BUFFER_END)
#define USB_BUFFER_PREV(X) ((X + USB_BUFFER_END) & USB_BUFFER_END)
// ^^^ THIS MUST EQUAL 2^n-1!!! Used as bitmask.

UsbPdu_t usbSendingQueue[USB_BUFFER_END + 1];
uint8_t usbSendingReadPos = 0;
uint8_t usbSendingWritePos = 0;

// How long (in system ticks) to wait for power to be disconnected
// Used to tell apart cable disconnect from USB suspend.
#define POWER_CHECK_DELAY 5000
uint16_t power_check_timer = 0;

CY_ISR_PROTO(Suspend_ISR);

void report_status(void) {
  memset(outbox.raw, 0, sizeof(outbox));
  outbox.response_type = C2RESPONSE_STATUS;
  outbox.payload[0] = status_register;
  outbox.payload[1] = DEVICE_VER_MAJOR;
  outbox.payload[2] = DEVICE_VER_MINOR;
  EEPROM_UpdateTemperature();
  outbox.payload[3] = dieTemperature[0];
  outbox.payload[4] = dieTemperature[1];
  usb_send_c2();
  // xprintf("time: %d", systime);
  // xprintf("LED status: %d %d %d %d %d", led_status&0x01, led_status&0x02,
  // led_status&0x04, led_status&0x08, led_status&0x10);
}

void process_ewo(OUT_c2packet_t *inbox) {
  status_register = inbox->payload[0];
  report_status();
  if (STATUS_IS(C2DEVSTATUS_SCAN_ENABLED)) {
    scan_start();
  }
}

void receive_config_block(OUT_c2packet_t *inbox) {
  if (status_register != (1 << C2DEVSTATUS_SETUP_MODE)) {
    xprintf("Invalid status register for config upload");
    return;
  }
  // TODO define offset via transfer block size and packet size
  memcpy(config.raw + (inbox->payload[0] * CONFIG_TRANSFER_BLOCK_SIZE),
         inbox->payload + CONFIG_BLOCK_DATA_OFFSET, CONFIG_TRANSFER_BLOCK_SIZE);
  memset(outbox.raw, 0, sizeof(outbox));
  outbox.response_type = C2RESPONSE_CONFIG;
  outbox.payload[0] = inbox->payload[0];
  usb_send_c2();
}

void send_config_block(OUT_c2packet_t *inbox) {
  memset(outbox.raw, 0, sizeof(outbox));
  outbox.response_type = C2RESPONSE_CONFIG;
  outbox.payload[0] = inbox->payload[0];
  memcpy(outbox.payload + CONFIG_BLOCK_DATA_OFFSET,
         config.raw + (inbox->payload[0] * CONFIG_TRANSFER_BLOCK_SIZE),
         CONFIG_TRANSFER_BLOCK_SIZE);
  usb_send_c2();
}

void sanitize_nvram_parameters(void) {
  if (config.matrixRows != MATRIX_ROWS || config.matrixCols != MATRIX_COLS) {
    // Yeehaw, a virgin EEPROM (or a different matrix size reflash)
    memset(config.thresholds, 0, sizeof(config.thresholds));
    config.configVersion = CS_CONFIG_VERSION;
  }
  config.matrixRows = MATRIX_ROWS;
  config.matrixCols = MATRIX_COLS;
  config.matrixLayers = MATRIX_LAYERS;
  config.switchType = SWITCH_TYPE;
  config.pedals = NUM_PEDALS;
  switch (config.adcBits) {
    case 8:
    case 10:
    case 12:
      break;
    default:
      // People don't read docs hence can't find this setting. 12 bit usually 
      // not enough to go overboard, so let's default to 12 bits and deal with
      // possible "help I'm gone 3-digit" if those ever happen.
      config.adcBits = 12; 
  };
  if (config.chargeDelay < MIN_CHARGE_DELAY) {
    config.chargeDelay = MIN_CHARGE_DELAY;
  };
  if (config.dischargeDelay < MIN_DISCHARGE_DELAY) {
    config.dischargeDelay = MIN_DISCHARGE_DELAY;
  }
  if (config.debouncingTicks < 1) {
    config.debouncingTicks = DEFAULT_DEBOUNCING_TICKS;
  }
}

void load_config(void) {
  EEPROM_Start();
  CyDelayUs(5);
  // Copypaste from EEPROM.c/EEPROM_ReadByte! Use with caution!
  uint8 interruptState;
  interruptState = CyEnterCriticalSection();
  /* Request access to EEPROM for reading.
  This is needed to reserve PHUB for read operation from EEPROM */
  CyEEPROM_ReadReserve();
  memcpy(config.raw, (void *)CYDEV_EE_BASE, CYDEV_EE_SIZE);
  /* Release EEPROM array */
  CyEEPROM_ReadRelease();
  CyExitCriticalSection(interruptState);
  EEPROM_Stop();
  sanitize_nvram_parameters();
  if (config.configVersion < CS_LAST_COMPATIBLE_NVRAM_VERSION) {
    status_register = (1 << C2DEVSTATUS_WRONG_NVRAM) |
                      (1 << C2DEVSTATUS_INSANE) |
                      (1 << C2DEVSTATUS_SETUP_MODE);
    xprintf("Incompatible NVRAM layout!");
    return;
  }
  if (config.configVersion != CS_CONFIG_VERSION) {
    xprintf("Old version of EEPROM - possibly unpredictable results.");
  }
}

void apply_config(void) {
  gpio_init();
  pipeline_init(); // calls scan_reset
  scan_init(config.debouncingTicks);
  scan_start();
}

void save_config(void) {
  sanitize_nvram_parameters();
  EEPROM_Start();
  CyDelayUs(5);
  EEPROM_UpdateTemperature();
  xprintf("Updating EEPROM GO!");
  uint16 bytes_modified = 0;
  for (uint16 i = 0; i < EEPROM_BYTESIZE; i++)
    if (config.raw[i] != EEPROM_ReadByte(i)) {
      EEPROM_WriteByte(config.raw[i], i);
      bytes_modified++;
    }
  EEPROM_Stop();
  xprintf("Written %d bytes!", bytes_modified);
}

void usb_receive(OUT_c2packet_t *inbox) {
  ticksToAutonomy = SETUP_TIMEOUT;
  memset(outbox.raw, 0x00, sizeof(outbox));
  // ALL COMMANDS MUST GENERATE A RESPONSE OVER USB!
  switch (inbox->command) {
  case C2CMD_EWO:
    process_ewo(inbox);
    break;
  case C2CMD_GET_STATUS:
    if (STATUS_IS(C2DEVSTATUS_INSANE)) {
      scan_report_insanity();
    }
    report_status();
    break;
  case C2CMD_SET_MODE:
    FORCE_BIT(status_register, C2DEVSTATUS_SETUP_MODE, inbox->payload[0]);
    report_status();
    break;
  case C2CMD_ENTER_BOOTLOADER:
    xprintf("Jumping to bootloader..");
    Boot_Load(); // Does not return, no need for break
  case C2CMD_UPLOAD_CONFIG:
    receive_config_block(inbox);
    break;
  case C2CMD_DOWNLOAD_CONFIG:
    send_config_block(inbox);
    break;
  case C2CMD_APPLY_CONFIG:
    xprintf("Applying config..");
    SET_BIT(status_register, C2DEVSTATUS_SETUP_MODE);
    apply_config();
    report_status();
    xprintf("success!");
    break;
  case C2CMD_COMMIT:
    save_config();
    break;
  case C2CMD_ROLLBACK:
    xprintf("Resetting..");
    CySoftwareReset(); // Does not return, no need for break.
  case C2CMD_ENABLE_TELEMETRY:
    FORCE_BIT(status_register, C2DEVSTATUS_TELEMETRY_MODE, inbox->payload[0]);
    scan_reset();
    break;
  default:
    break;
  }
}

void usbEnqueue(uint8_t EP, uint8_t len, uint8_t *data) {
  usbSendingWritePos = USB_BUFFER_NEXT(usbSendingWritePos);
  usbSendingQueue[usbSendingWritePos].EP = EP;
  usbSendingQueue[usbSendingWritePos].len = len;
  memcpy(usbSendingQueue[usbSendingWritePos].data, data, len);
}

void usbSend() {
  if (usb_status != USB_STATUS_CONNECTED) {
    return;
  }
  while (usbSendingWritePos != usbSendingReadPos) {
    uint8_t pos = USB_BUFFER_NEXT(usbSendingReadPos);
    if (USB_GetEPState(usbSendingQueue[pos].EP) == USB_IN_BUFFER_EMPTY) {
      USB_LoadInEP(usbSendingQueue[pos].EP,
          usbSendingQueue[pos].data, usbSendingQueue[pos].len);
      usbSendingReadPos = pos;
    } else {
      break;
    }
  }
}

void usb_send_c2(void) {
  usbEnqueue(OUTBOX_EP, sizeof(outbox.raw), outbox.raw);
}

void usb_send_c2_blocking(void) {
  usb_send_c2();
  while (usbSendingReadPos != usbSendingWritePos) {
    usbSend();
  }
}

// Very similar to consumer_press, but keycode there is uint16_t :(
inline void keyboard_press(uint8_t keycode) {
  if ((keycode & 0xf8) == 0xe0) {
    keyboard_report.mods |= (1 << (keycode & 0x07));
    return;
  }
  for (uint8_t cur_pos = 0; cur_pos < sizeof keyboard_report.keys; cur_pos++) {
    if (keyboard_report.keys[cur_pos] == keycode) {
      xprintf("Existing %d pos %d", keycode, cur_pos);
      return;
    } else if (keyboard_report.keys[cur_pos] == 0) {
      keyboard_report.keys[cur_pos] = keycode;
      keys_pressed = cur_pos + 1;
      // xprintf("Pressed %d pos %d, usage %d", keycode, cur_pos,
      // keys_pressed);
      return;
    }
  }
}

// Very similar to consumer_release, but keycode there is uint16_t :(
inline void keyboard_release(uint8_t keycode) {
  if ((keycode & 0xf8) == 0xe0) {
    keyboard_report.mods &= ~(1 << (keycode & 0x07));
    return;
  }
  for (uint8_t cur_pos = 0; cur_pos < sizeof keyboard_report.keys; cur_pos++) {
    if (keyboard_report.keys[cur_pos] == keycode) {
      // Key was, in fact, pressed.
      keyboard_report.keys[cur_pos] = keyboard_report.keys[--keys_pressed];
      keyboard_report.keys[keys_pressed] = 0;
      // xprintf("Released %d, usage %d", keycode, keys_pressed);
      return;
    }
  }
}

void update_keyboard_report(hid_event* event) {
  // xprintf("Updating report for %d", key->keycode);
  if ((event->flags & HID_RELEASED_MASK) == 0) {
    keyboard_press(event->code);
  } else {
    keyboard_release(event->code);
  }
  memcpy(KBD_OUTBOX, keyboard_report.raw, OUTBOX_SIZE(KBD_OUTBOX));
  if (keys_pressed > KBD_KRO_LIMIT) {
    // on rollover error ALL keys must report ERO (Error: Rollover).
    memset(KBD_OUTBOX + 2, 1, KBD_KRO_LIMIT); // ERO is scancode 1.
    xprintf("Keyboard rollover error");
  }
  USB_SEND_REPORT(KBD);
}

const uint16_t consumer_mapping[16] = {
    0xcd, // Play/pause
    0xe2, // Mute
    0xe9, // Vol++
    0xea, // Vol--
    0xb8, // Eject
    0x00, 0x00, 0x00,
    // 8
    0xb0, // Play
    0xb1, // Pause
    0xb2, // Record
    0xb3, // Fwd
    0xb4, // Rev
    0xb5, // NTrk
    0xb6, // PTrk
    0xb7  // Stop
    // 16
};

// Very similar to keyboard_press, but keycode there is uint8_t :(
static inline void consumer_press(uint16_t code) {
  for (uint8_t cur_pos = 0; cur_pos < CONSUMER_KRO_LIMIT; cur_pos++) {
    if (consumer_report[cur_pos] == code) {
      xprintf("Existing %d pos %d", code, cur_pos);
      break;
    } else if (consumer_report[cur_pos] == 0) {
      consumer_report[cur_pos] = code;
      // xprintf("Pressed %d pos %d", code, cur_pos);
      break;
    }
  }
  // xprintf("C_Pressing %d", code);
}

static inline void consumer_release(uint16_t keycode) {
  uint8_t cur_pos;
  bool move = false;
  for (cur_pos = 0; cur_pos < CONSUMER_KRO_LIMIT; cur_pos++) {
    if (move) {
      consumer_report[cur_pos - 1] = consumer_report[cur_pos];
    } else if (consumer_report[cur_pos] == keycode) {
      move = true;
    }
  }
  if (move) {
    // Key was, in fact, pressed.
    consumer_report[CONSUMER_KRO_LIMIT - 1] = 0;
    // xprintf("C_Released %d", keycode);
  }
}

void update_consumer_report(hid_event* event) {
  // xprintf("Updating report for %d", key->keycode);
  uint16_t code = consumer_mapping[event->code - 0xe8];
  if ((event->flags & HID_RELEASED_MASK) == 0) {
    consumer_press(code);
  } else {
    consumer_release(code);
  }
  memcpy(CONSUMER_OUTBOX, consumer_report, OUTBOX_SIZE(CONSUMER_OUTBOX));
  USB_SEND_REPORT(CONSUMER);
}

void update_system_report(hid_event* event) {
  uint8_t keyIndex = event->code - 0xa5;
  if ((event->flags & HID_RELEASED_MASK) == 0) {
    system_report[0] |= (1 << keyIndex);
  } else {
    system_report[0] &= ~(1 << keyIndex);
  }
  memcpy(SYSTEM_OUTBOX, system_report, OUTBOX_SIZE(SYSTEM_OUTBOX));
  // xprintf("System: %d", SYSTEM_OUTBOX[0]);
  USB_SEND_REPORT(SYSTEM);
}

void usb_suspend_monitor_start(void) {
  SuspendWD_Stop();
  SuspendWD_WriteCounter(0);
  SuspendWD_Start();
  USBSuspendIRQ_StartEx(
      Suspend_ISR); // Does disabling for you, safe to use that way.
}

void usb_suspend_monitor_stop(void) {
  SuspendWD_Stop();
  USBSuspendIRQ_Stop();
}

void usb_init(void) {
  pipeline_init();
  memset(keyboard_report.raw, 0, sizeof keyboard_report.raw);
  keys_pressed = 0;
  memset(consumer_report, 0, sizeof consumer_report);
#ifndef SELF_POWERED
  USB_Start(0u, USB_5V_OPERATION);
#endif
}

void usb_configure(void) {
  memset(KBD_OUTBOX, 0, sizeof(KBD_OUTBOX));
  memset(CONSUMER_OUTBOX, 0, sizeof(CONSUMER_OUTBOX));
  memset(SYSTEM_OUTBOX, 0, sizeof(SYSTEM_OUTBOX));
  if (0u == USB_GetConfiguration()) {
    // This never happens. But let's handle it just in case.
    usb_status = USB_STATUS_DISCONNECTED;
    output_direction = OUTPUT_DIRECTION_SERIAL;
#ifdef SELF_POWERED
    CyPins_ClearPin(HPWR_0);
#endif
  } else {
    usb_status = USB_STATUS_CONNECTED;
    output_direction = OUTPUT_DIRECTION_USB;
    usb_suspend_monitor_start();
#ifdef SELF_POWERED
    CyPins_SetPin(HPWR_0);
#endif
  }
}

uint8_t usb_powered() {
#ifdef SELF_POWERED
  return USB_VBusPresent();
#else
  return 1;
#endif
}

void usb_tick(void) {
#ifdef SELF_POWERED
  if (usb_powered()) {
    if (USB_initVar == 0) {
      CyPins_ClearPin(HPWR_0);
      USB_Start(0u, USB_POWER_MODE);
    }
  } else {
    if (usb_status == USB_STATUS_CONNECTED) {
      CyPins_ClearPin(HPWR_0);
      USB_Stop();
      usb_status = USB_STATUS_DISCONNECTED;
      output_direction = OUTPUT_DIRECTION_SERIAL;
    }
    return;
  }
#endif
  if (0u != USB_IsConfigurationChanged()) {
    usb_configure();
  }
  uint8_t enableInterrupts = CyEnterCriticalSection();
  if (CTRLR_SCB.status == USB_XFER_STATUS_ACK) {
    CTRLR_SCB.status = USB_XFER_IDLE;
    usb_receive((OUT_c2packet_t*)CTRLR_INBOX);
  }
  if (KBD_SCB.status == USB_XFER_STATUS_ACK) {
    led_status = KBD_INBOX[0];
    KBD_SCB.status = USB_XFER_IDLE;
    gpio_setLEDs(led_status);
  }
  CyExitCriticalSection(enableInterrupts);
  usbSend();
}

#define RESET_SINGLE(R, O)                                                     \
  memset(R, 0, sizeof R);                                                      \
  if (memcmp(R, O##_OUTBOX, OUTBOX_SIZE(O##_OUTBOX)) != 0) {                   \
    memset(O##_OUTBOX, 0, OUTBOX_SIZE(O##_OUTBOX));                            \
    USB_SEND_REPORT(O);                                                        \
  }

/*
 * updates host only if there were stuck keys, to avoid N reports on every "last
 * key depressed".
 */
void reset_reports(void) {
  RESET_SINGLE(keyboard_report.raw, KBD)
  keys_pressed = 0;
  RESET_SINGLE(consumer_report, CONSUMER)
  RESET_SINGLE(system_report, SYSTEM)
  // xprintf("reports reset");
}

void usb_nap(void) {
  // TODO reconfigure monitor period to provide periodic wakeups for
  // monitor-in-suspend
  usb_suspend_monitor_stop();
  power_check_timer = POWER_CHECK_DELAY;
  wakeup_enabled = USB_RWUEnabled();
  USB_Suspend();
  power_state = DEVSTATE_PREPARING_TO_SLEEP;
#ifdef SELF_POWERED
  CyPins_ClearPin(HPWR_0); // Actually, SUSP pin should be used here
#endif
  // But there's no SUSP pin laid out.
}

void usb_check_power(void) {
#ifndef SELF_POWERED
  if (0) { //Skip the first option.
#else
  if (power_check_timer-- > 0) {
    if (usb_powered()) {
      return;
    }
    // power disconnected. No point waiting further.
    power_check_timer = 0;
  }
  if (usb_powered() == 0) {
#endif
    // USB disconnected, not a suspend. Return to full power.
    power_state = DEVSTATE_FULL_THROTTLE;
    scan_start();
  } else if (wakeup_enabled == 0) {
    power_state = DEVSTATE_SLEEP;
    CyPmAltAct(PM_ALT_ACT_TIME_NONE, PM_ALT_ACT_SRC_NONE);
  } else {
    power_state = DEVSTATE_WATCH;
  }
}

void usb_wake(void) {
  USB_Resume();
  power_state = DEVSTATE_FULL_THROTTLE;
  scan_start();
  usb_suspend_monitor_start();
  // CyIMO_SetFreq(CY_IMO_FREQ_USB);
}

void usb_send_wakeup(void) {
  CyDelay(5); // Just in case, not to violate spec by waking immediately.
  usb_wake();
  usb_suspend_monitor_stop();
  USB_Force(USB_FORCE_K);
  CyDelay(5);
  USB_Force(USB_FORCE_NONE);
  /*
   * Host must send resume for at least 20ms (USB 2.0 spec 7.1.7.7).
   * So, 15 more.
   * We also officially have 10ms to wake.
   * Let's sit here a bit longer
   * so we don't have to worry about suspend watchdog shutting us down.
   */
  CyDelay(15 + 2);
  usb_suspend_monitor_start();
}

CY_ISR(Suspend_ISR) {
#ifdef DEBUG_INTERRUPTS
  PIN_DEBUG(1, 5)
#endif
  if (power_state == DEVSTATE_SUSPENDING) {
    return;
  }
  //"USB_Dp_Read()" != 0 && USB_Dm_Read() == 0
  if ((USB_Dp_PS & (USB_Dp__MASK | USB_Dm__MASK)) == USB_Dp__MASK) {
    // Suspend is when no activity for 3ms and J (=Dp is high)
    power_state = DEVSTATE_SUSPENDING;
  }
  // bus reset while awake is handled by component.
  // suspend state is handled by DP ISR
}

void USB_DP_ISR_EntryCallback(void) {
#ifdef DEBUG_INTERRUPTS
  PIN_DEBUG(1, 4)
#endif
  if (power_state == DEVSTATE_RESUMING) {
    return;
  }
  power_state = DEVSTATE_RESUMING;
}

void xprintf(const char *format_p, ...) {
#ifndef XPRINTF_ALWAYS_ENABLED
  if (BIT_IS_CLEAR(status_register, C2DEVSTATUS_SETUP_MODE)) {
    return;
  }
#endif
  va_list va;
  va_start(va, format_p);
  vsnprintf((char *)outbox.raw, sizeof(outbox), format_p, va);
  va_end(va);
  usb_send_c2();
}

// Auto-timestamped xprintf
void ts_xprintf(const char *format_p, ...) {
#ifndef XPRINTF_ALWAYS_ENABLED
  if (BIT_IS_CLEAR(status_register, C2DEVSTATUS_SETUP_MODE)) {
    return;
  }
#endif
  va_list va;
  va_start(va, format_p);
  log_message_t* msg = (log_message_t*)&outbox.raw;
  vsnprintf(msg->message, sizeof(msg->message), format_p, va);
  va_end(va);
  msg->response_type = C2RESPONSE_LOG_MESSAGE;
  msg->sysTime = systime;
  usb_send_c2();
}


void coded_timestamped_message(uint8_t messageCode) {
#ifndef CODED_MESSAGES_ALWAYS_ENABLED
  if (BIT_IS_CLEAR(status_register, C2DEVSTATUS_SETUP_MODE)) {
    return;
  }
#endif
  coded_message_t* cm = (coded_message_t*)&outbox.raw;
  cm->response_type = C2RESPONSE_CODED_MESSAGE;
  cm->messageCode = messageCode;
  cm->sysTime = systime;
  usb_send_c2();
}
