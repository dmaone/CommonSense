/*
 *
 * Copyright (C) 2016-2017 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "../io.h"

#include <project.h>
#include <stdio.h>

#include "../gpio.h"
#include "../globals.h"
#include "../scan.h"
#include "../settings.h"

// for xprintf - stdio + stdarg
#include <stdarg.h>

typedef struct {
  uint8_t EP;
  uint8_t len;
  uint8_t data[64];
} UsbPdu_t;

#define USB_BUFFER_END 15
#define USB_BUFFER_NEXT(X) ((X + 1) & USB_BUFFER_END)
#define USB_BUFFER_PREV(X) ((X + USB_BUFFER_END) & USB_BUFFER_END)
// ^^^ THIS MUST EQUAL 2^n-1!!! Used as bitmask.

#define KBD_SCB USB_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_OUT_RPT_SCB
#define KBD_INBOX USB_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_OUT_BUF
#define KBD_OUTBOX USB_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_IN_BUF

#define CONSUMER_OUTBOX                                                        \
  USB_DEVICE0_CONFIGURATION0_INTERFACE2_ALTERNATE0_HID_IN_BUF

#define SYSTEM_OUTBOX                                                          \
  USB_DEVICE0_CONFIGURATION0_INTERFACE3_ALTERNATE0_HID_IN_BUF

#define CTRLR_SCB                                                              \
  USB_DEVICE0_CONFIGURATION0_INTERFACE1_ALTERNATE0_HID_OUT_RPT_SCB
#define CTRLR_INBOX USB_DEVICE0_CONFIGURATION0_INTERFACE1_ALTERNATE0_HID_OUT_BUF

UsbPdu_t usbSendingQueue[USB_BUFFER_END + 1];
uint8_t usbSendingReadPos = 0;
uint8_t usbSendingWritePos = 0;

void report_status(void) {
  IN_c2packet_t outbox;
  outbox.response_type = C2RESPONSE_STATUS;
  outbox.payload[0] = status_register;
  outbox.payload[1] = DEVICE_VER_MAJOR;
  outbox.payload[2] = DEVICE_VER_MINOR;
  EEPROM_UpdateTemperature();
  outbox.payload[3] = dieTemperature[0];
  outbox.payload[4] = dieTemperature[1];
  io_c2(&outbox);
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

void usb_receive(OUT_c2packet_t *inbox) {
  ticksToAutonomy = SETUP_TIMEOUT;
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
    settings_receive_block(inbox);
    break;
  case C2CMD_DOWNLOAD_CONFIG:
    settings_send_block(inbox);
    break;
  case C2CMD_APPLY_CONFIG:
    xprintf("Applying config..");
    SET_BIT(status_register, C2DEVSTATUS_SETUP_MODE);
    settings_apply();
    report_status();
    xprintf("success!");
    break;
  case C2CMD_COMMIT:
    settings_save();
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

void io_init(void) {
  pipeline_init();
#ifndef SELF_POWERED
  USB_Start(0u, USB_5V_OPERATION);
#endif
}

void io_tick(void) {
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

void xprintf(const char *format_p, ...) {
#ifndef XPRINTF_ALWAYS_ENABLED
  if (BIT_IS_CLEAR(status_register, C2DEVSTATUS_SETUP_MODE)) {
    return;
  }
#endif
  IN_c2packet_t outbox;
  va_list va;
  va_start(va, format_p);
  vsnprintf((char *)outbox.raw, sizeof(outbox), format_p, va);
  va_end(va);
  io_c2(&outbox);
}

// Auto-timestamped xprintf
void ts_xprintf(const char *format_p, ...) {
#ifndef XPRINTF_ALWAYS_ENABLED
  if (BIT_IS_CLEAR(status_register, C2DEVSTATUS_SETUP_MODE)) {
    return;
  }
#endif
  IN_c2packet_t outbox;
  va_list va;
  va_start(va, format_p);
  log_message_t* msg = (log_message_t*)&outbox.raw;
  vsnprintf(msg->message, sizeof(msg->message), format_p, va);
  va_end(va);
  msg->response_type = C2RESPONSE_LOG_MESSAGE;
  msg->sysTime = systime;
  io_c2(&outbox);
}


void coded_timestamped_message(coded_message_t* msg) {
#ifndef CODED_MESSAGES_ALWAYS_ENABLED
  if (BIT_IS_CLEAR(status_register, C2DEVSTATUS_SETUP_MODE)) {
    return;
  }
#endif
  msg->response_type = C2RESPONSE_CODED_MESSAGE;
  msg->sysTime = systime;
  io_c2((IN_c2packet_t*)msg);
}

#if NOT_A_KEYBOARD == 1
#define _WIPE_OUTBOX(OUTBOX) memset(OUTBOX, 0, OUTBOX_SIZE(OUTBOX))
#else
#define _WIPE_OUTBOX(OUTBOX)
#endif

#define USB_SEND_REPORT(TYPE)                                                  \
  _WIPE_OUTBOX(TYPE##_OUTBOX);                                                 \
  usbEnqueue(TYPE##_EP, OUTBOX_SIZE(TYPE##_OUTBOX), data);

inline void io_keyboard(void* data) {
  USB_SEND_REPORT(KBD);
}

inline void io_consumer(void* data) {
  USB_SEND_REPORT(CONSUMER);
}

inline void io_system(void* data) {
  USB_SEND_REPORT(SYSTEM);
}

void io_c2(IN_c2packet_t* data) {
  usbEnqueue(OUTBOX_EP, sizeof(data->raw), data->raw);
}

void io_c2_blocking(IN_c2packet_t* data) {
  io_c2(data);
  while (usbSendingReadPos != usbSendingWritePos) {
    usbSend();
  }
}
