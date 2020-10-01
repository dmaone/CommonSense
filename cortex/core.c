/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "core.h"

#include <project.h>

#include "gpio.h"
#include "globals.h"
#include "pipeline.h"
#include "PSoC_USB.h"
#include "scan.h"
#include "sup_serial.h"

static volatile uint8_t pending_usb_ticks;

CY_ISR(BootIRQ_ISR) {
  Boot_Load();
}

CY_ISR(Timer_ISR) {
  pending_usb_ticks++;
  systime++;
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

  load_config();

  power_state = DEVSTATE_FULL_THROTTLE;
  output_direction = OUTPUT_DIRECTION_SERIAL;
  status_register = 0;
  FORCE_BIT(status_register, C2DEVSTATUS_SETUP_MODE, NOT_A_KEYBOARD);
  serial_init();
  usb_init();
  apply_config();
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
        if (--ticksToAutonomy < 0) {
          // If FlightController doesn't poll us anymore - go to normal mode.
          ticksToAutonomy = LARGE_ENOUGH_32;

          status_register = 0; // Clear EVERYTHING
          scan_common_start(SANITY_CHECK_DURATION); // pipeline init doesn't

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
      usb_tick();
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
      //usb_nap();
      CyPmSaveClocks();
      CyPmSleep(PM_SLEEP_TIME_NONE, PM_SLEEP_SRC_I2C|PM_SLEEP_SRC_PICU);
      CyPmRestoreClocks();
      SysTimer_Wakeup();
      serial_wake();
      Sup_Pdu_t buf;
      buf.command = 4;
      buf.data = 5;
      serial_send(&buf);
      //usb_wake();
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
