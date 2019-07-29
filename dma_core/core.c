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

#include "exp.h"
#include "globals.h"
#include "pipeline.h"
#include "PSoC_USB.h"
#include "scan_common.h"
#include "sup_serial.h"

#if SCANNER_TYPE == SCANNER_SUN
#include "scanner_sun.h"
#elif SCANNER_TYPE == SCANNER_ADB
#include "scanner_adb.h"
#else
#include "scan.h"
#endif

CY_ISR(BootIRQ_ISR) {
  Boot_Load();
}

CY_ISR(Timer_ISR) {
  tick++;
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
      if (tick) {
        exp_tick(tick);
        tick = 0;
        if (sanity_check_timer > 0) {
          scan_sanity_check();
        }
        scan_tick();
        if (--ticksToAutonomy < 0) {
          // If FlightController doesn't poll us anymore - go to normal mode.
          ticksToAutonomy = LARGE_ENOUGH_32; // also disables xprintf!
          CLEAR_BIT(status_register, C2DEVSTATUS_SETUP_MODE);
          // Should reapply config - but firmware seems to crash if I do.
          // apply_config();
        }
        if (TEST_BIT(status_register, C2DEVSTATUS_MATRIX_MONITOR)) {
          report_matrix_readouts();
        } else if (TEST_BIT(status_register, C2DEVSTATUS_OUTPUT_ENABLED)) {
          pipeline_process();
        }
      }
      serial_tick();
      usb_tick();
      // Timer ISR will wake us up.
      CyPmAltAct(PM_ALT_ACT_TIME_NONE, PM_ALT_ACT_SRC_NONE);
      break;
    case DEVSTATE_PREPARING_TO_SLEEP:
      if (tick) {
        tick = 0;
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
      if (tick > SUSPEND_SYSTIMER_DIVISOR) {
        tick = 0;
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
      tick = 0;
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
