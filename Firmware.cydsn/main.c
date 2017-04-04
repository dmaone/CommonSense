/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
*/
#include "globals.h"
#include <project.h>
#include "PSoC_USB.h"
#include "c2/c2_protocol.h"
#include "c2/nvram.h"
#include "scan.h"
#include "pipeline.h"

CY_ISR(BootIRQ_ISR)
{
    Boot_Load();
}

CY_ISR(Timer_ISR)
{
    tick++;
    systime++;
}

int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    BootIRQ_StartEx(BootIRQ_ISR);
    SysTimer_WritePeriod(BCLK__BUS_CLK__KHZ); // Need 1kHz
    SysTimer_Start();
    TimerIRQ_StartEx(Timer_ISR);

    load_config();

    status_register.matrix_output = 0;
    status_register.emergency_stop = 0;
    status_register.setup_mode = NOT_A_KEYBOARD;
    usb_init();
    scan_init();
    pipeline_init(); // calls scan_reset
    scan_start(); // We are starting in full power - must do that initial kick
    for(;;)
    {
        switch (power_state)
        {
            case DEVSTATE_FULL_THROTTLE:
                if (tick)
                {
                    tick = 0;
                    if (0u != USB_IsConfigurationChanged())
                    {
                        usb_configure();
                    }
                    if (CTRLR_SCB.status == USB_XFER_STATUS_ACK)
                    {
CyPins_SetPin(ExpHdr_2);
CyDelayUs(4);
CyPins_ClearPin(ExpHdr_2);
                        process_msg((void*)CTRLR_INBOX);
                        CTRLR_SCB.status = USB_XFER_IDLE;
                    }
                    if (KBD_SCB.status == USB_XFER_STATUS_ACK)
                    {
                        uint8_t enableInterrupts = CyEnterCriticalSection();
                        led_status = KBD_INBOX[0];
                        KBD_SCB.status = USB_XFER_IDLE;
                        CyExitCriticalSection(enableInterrupts);
                    }
                    if (status_register.matrix_output > 0)
                        report_matrix_readouts();
                    pipeline_process();
                }
                // Timer ISR will wake us up.
                CyPmAltAct(PM_ALT_ACT_TIME_NONE, PM_ALT_ACT_SRC_NONE);
                break;
            case DEVSTATE_SLEEP:
                // We're supposed to be in deeper sleep so not ever getting here.
                // But if not - keep things warm, sleep the CPU.
                CyPmAltAct(PM_ALT_ACT_TIME_NONE, PM_ALT_ACT_SRC_NONE);
                break;
            case DEVSTATE_WATCH:
                if (tick > SUSPEND_SYSTIMER_DIVISOR)
                {
                    tick = 0;
                    scan_start();
                    if (pipeline_process_wakeup())
                    {
                        usb_send_wakeup();
                    }
                }
                CyPmAltAct(PM_ALT_ACT_TIME_NONE, PM_ALT_ACT_SRC_NONE);
                break;
            case DEVSTATE_SUSPENDING:
                nap();
                break;
            case DEVSTATE_RESUMING:
                tick = 0;
                wake();
                break;
            default:
                // Stray interrupt? 
                // We'd better stay awake and sort it out next iteration.
                break;
        }
    }
}

/* [] END OF FILE */
