/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "PSoC_USB.h"
#include "exp.h"
#include "globals.h"
#include "pipeline.h"
#include "scan.h"
#include <project.h>

CY_ISR(BootIRQ_ISR) { Boot_Load(); }

CY_ISR(Timer_ISR) {
  tick++;
  systime++;
}

int main() {
  CyGlobalIntEnable; /* Enable global interrupts. */
  BootIRQ_StartEx(BootIRQ_ISR);
  SysTimer_WritePeriod(BCLK__BUS_CLK__KHZ); // Need 1kHz
  SysTimer_Start();
  TimerIRQ_StartEx(Timer_ISR);

  load_config();

  status_register = 0;
  FORCE_BIT(status_register, C2DEVSTATUS_SETUP_MODE, NOT_A_KEYBOARD);
  usb_init();
  scan_init();
  apply_config();
  scan_start(); // We are starting in full power - must do that initial kick
  for (;;) {
    switch (power_state) {
    case DEVSTATE_FULL_THROTTLE:
#ifdef DEBUG_STATE_MACHINE
      PIN_DEBUG(2, 1)
#endif
      if (tick) {
        exp_tick(tick);
        tick = 0;
        if (sanity_check_timer > 0) {
          scan_sanity_check();
          if (0 == sanity_check_timer) {
            OUT_c2packet_t inbox;
            inbox.command = C2CMD_GET_STATUS;
            process_msg(&inbox);
          }
        }
        if (0u != USB_IsConfigurationChanged()) {
          usb_configure();
        }
        uint8_t enableInterrupts = CyEnterCriticalSection();
        if (CTRLR_SCB.status == USB_XFER_STATUS_ACK) {
          static OUT_c2packet_t inbox;
          memcpy(&inbox, (void *)CTRLR_INBOX, sizeof(inbox));
          CTRLR_SCB.status = USB_XFER_IDLE;
          // processing message sends USB packets, interrupts are vital.
          CyExitCriticalSection(enableInterrupts);
          process_msg(&inbox);
          enableInterrupts = CyEnterCriticalSection();
        }
        if (KBD_SCB.status == USB_XFER_STATUS_ACK) {
          led_status = KBD_INBOX[0];
          KBD_SCB.status = USB_XFER_IDLE;
          exp_setLEDs(led_status);
        }
        CyExitCriticalSection(enableInterrupts);
        if (TEST_BIT(status_register, C2DEVSTATUS_MATRIX_MONITOR)) {
          report_matrix_readouts();
        } else if (TEST_BIT(status_register, C2DEVSTATUS_OUTPUT_ENABLED)) {
          pipeline_process();
        }
      }
      // Timer ISR will wake us up.
      CyPmAltAct(PM_ALT_ACT_TIME_NONE, PM_ALT_ACT_SRC_NONE);
      break;
    case DEVSTATE_SLEEP:
#ifdef DEBUG_STATE_MACHINE
      PIN_DEBUG(2, 2)
#endif
      // We're supposed to be in deeper sleep so not ever getting here.
      // But if not - keep things warm, sleep the CPU.
      CyPmAltAct(PM_ALT_ACT_TIME_NONE, PM_ALT_ACT_SRC_NONE);
      break;
    case DEVSTATE_WATCH:
#ifdef DEBUG_STATE_MACHINE
      PIN_DEBUG(2, 3)
#endif
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
#ifdef DEBUG_STATE_MACHINE
      PIN_DEBUG(2, 4)
#endif
      nap();
      break;
    case DEVSTATE_RESUMING:
#ifdef DEBUG_STATE_MACHINE
      PIN_DEBUG(2, 5)
#endif
      // only from deep sleep - RWU goes straight to full throttle.
      tick = 0;
      wake();
      break;
    default:
#ifdef DEBUG_STATE_MACHINE
      PIN_DEBUG(2, 6)
#endif
      // Stray interrupt?
      // We'd better stay awake and sort it out next iteration.
      break;
    }
  }
}

/* [] END OF FILE */
