/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "../core.h"

#include <project.h>

#include "../gpio.h"
#include "../globals.h"
#include "../pipeline.h"
#include "../io.h"
#include "../hid.h"
#include "../scan.h"
#include "../settings.h"
#include "../sup_serial.h"

static volatile uint8_t pending_usb_ticks;
uint8_t usb_status = USB_STATUS_DISCONNECTED;
uint8_t usb_wakeup_enabled = 0;

#define SUSPEND_SYSTIMER_DIVISOR 10

// How long (in system ticks) to wait for power to be disconnected
// Used to tell apart cable disconnect from USB suspend.
#define POWER_CHECK_DELAY 5000
uint16_t power_check_timer = 0;

CY_ISR(BootIRQ_ISR) {
  Boot_Load();
}

CY_ISR(Timer_ISR) {
  pending_usb_ticks++;
  systime++;
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


void usb_init(void);
void usb_configure(void);
void usb_nap(void);
void usb_check_power(void);
void usb_wake(void);

void usb_send_wakeup(void);

void usb_suspend_monitor_start(void) {
  SuspendWD_Stop();
  SuspendWD_WriteCounter(0);
  SuspendWD_Start();
  USBSuspendIRQ_StartEx(Suspend_ISR); // Auto-disables, safe to use like this.
}

void usb_suspend_monitor_stop(void) {
  SuspendWD_Stop();
  USBSuspendIRQ_Stop();
}

void usb_configure(void) {
  hid_reset_reports();
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

void usb_nap(void) {
  // TODO reconfigure monitor period to provide periodic wakeups for
  // monitor-in-suspend
  usb_suspend_monitor_stop();
  power_check_timer = POWER_CHECK_DELAY;
  usb_wakeup_enabled = USB_RWUEnabled();
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
  } else if (usb_wakeup_enabled == 0) {
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

inline void setup() {
#ifdef EXTERNAL_CORE_POWER
  // Disable core internal LDOs.
  CY_PM_PWRSYS_CR0_REG = 0b00110000;
#endif
  ILO_Trim_Start();
  ILO_Trim_BeginTrimming();
  CyGlobalIntEnable; /* Enable global interrupts. */
  BootIRQ_StartEx(BootIRQ_ISR);
  SysTimer_Start();
  TimerIRQ_StartEx(Timer_ISR);

  settings_load();

  power_state = DEVSTATE_FULL_THROTTLE;
  output_direction = OUTPUT_DIRECTION_SERIAL;
  status_register = 0;
  FORCE_BIT(status_register, C2DEVSTATUS_SETUP_MODE, NOT_A_KEYBOARD);
  serial_init();
  io_init();
  settings_apply();
  scan_start(); // We are starting in full power - must do that initial kick
}

inline void main_loop() {
  for (;;) {
#ifdef DEBUG_STATE_MACHINE
      PIN_DEBUG(2, power_state)
#endif
    switch (power_state) {
    case DEVSTATE_FULL_THROTTLE:
      if (pending_usb_ticks > 0) {
        gpio_tick(pending_usb_ticks);
        pending_usb_ticks = 0;
        if (sanity_check_timer > 0) {
          scan_sanity_check();
        }
        scan_tick();
        if ((ticksToAutonomy <= SETUP_TIMEOUT) && (--ticksToAutonomy < 0)) {
          // If FlightController doesn't poll us anymore - go to normal mode.
          ticksToAutonomy = LARGE_ENOUGH_32;

          status_register = 0; // Clear EVERYTHING
          CyDelay(10); // Ensure scan cycle ended by interrupt handler
          scan_start();
          pipeline_init();
          // Should reapply config - but firmware seems to crash if I do.
          // apply_config();
        }
        if (STATUS_IS(C2DEVSTATUS_TELEMETRY_MODE)) {
          report_matrix_readouts();
        } else if (STATUS_IS(C2DEVSTATUS_OUTPUT_ENABLED)) {
          pipeline_process();
        }
      }
      serial_tick();
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

      io_tick();
      // Timer ISR will wake us up.
      CyPmAltAct(PM_ALT_ACT_TIME_NONE, PM_ALT_ACT_SRC_NONE);
      break;
    case DEVSTATE_PREPARING_TO_SLEEP:
      if (pending_usb_ticks) {
        pending_usb_ticks = 0;
        // Changes power state!
        usb_check_power();
      }
      break;
    case DEVSTATE_SLEEP:
      // We're supposed to be in deeper sleep so not ever getting here.
      // But if not - keep things warm, sleep the CPU.
      CyPmAltAct(PM_ALT_ACT_TIME_NONE, PM_ALT_ACT_SRC_NONE);
      break;
    case DEVSTATE_WATCH:
      if (pending_usb_ticks > SUSPEND_SYSTIMER_DIVISOR) {
        pending_usb_ticks = 0;
        scan_start();
        if (pipeline_process_wakeup()) {
          usb_send_wakeup();
        }
      }
      CyPmAltAct(PM_ALT_ACT_TIME_NONE, PM_ALT_ACT_SRC_NONE);
      break;
    case DEVSTATE_SUSPENDING:
      usb_nap();
      break;
    case DEVSTATE_RESUMING:
      // only from deep sleep - RWU goes straight to full throttle.
      pending_usb_ticks = 0;
      usb_wake();
      break;
    case DEVSTATE_SLEEP_REQUEST:
      scan_nap();
      serial_nap();
      SysTimer_Sleep();
      usb_nap();
      CyPmSaveClocks();
      CyPmSleep(PM_SLEEP_TIME_NONE, PM_SLEEP_SRC_I2C|PM_SLEEP_SRC_PICU);
      CyPmRestoreClocks();
      SysTimer_Wakeup();
      serial_wake();
      Sup_Pdu_t buf;
      buf.command = 4;
      buf.data = 5;
      serial_send(&buf);
      usb_wake();
      scan_wake();
      power_state = DEVSTATE_FULL_THROTTLE;
      break;
    default:
#ifdef DEBUG_STATE_MACHINE
      PIN_DEBUG(2, DEVSTATE_MAX)
#endif
      // Stray interrupt?
      // We'd better stay awake and sort it out next iteration.
      break;
    }
  }
}
