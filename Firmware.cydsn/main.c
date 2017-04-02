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

void nap(void)
{
    USB_Suspend();
    if (USB_RWUEnabled() == 0)
    {
        power_state = DEVSTATE_SLEEP;
        CyPmAltAct(PM_ALT_ACT_TIME_NONE, PM_ALT_ACT_SRC_NONE);
    }
    else
    {
        power_state = DEVSTATE_WATCH;
        CyIMO_SetFreq(CY_IMO_FREQ_3MHZ);
// CyFlash_SetWaitCycles(3);
// CyDelayFreq(3000000); - not supposed to be used, busy loops are evil
    }
}

void wake(void)
{
    power_state = DEVSTATE_FULL_THROTTLE;
    CyIMO_SetFreq(CY_IMO_FREQ_USB);
    scan_start();
    USB_Resume();
}

int main()
{
    BootIRQ_StartEx(BootIRQ_ISR);
    CyGlobalIntEnable; /* Enable global interrupts. */
    load_config();
    status_register.matrix_output = 0;
    status_register.emergency_stop = 0;
    status_register.setup_mode = NOT_A_KEYBOARD;
    usb_init();
    scan_init();
    pipeline_init(); // calls scan_reset
    scan_start(); // We are starting in full power - must do that initial kick
    SysTimer_WritePeriod(BCLK__BUS_CLK__KHZ); // Need 1kHz
    SysTimer_Start();
    TimerIRQ_StartEx(Timer_ISR);
    for(;;)
    {
        switch (power_state)
        {
            case DEVSTATE_FULL_THROTTLE:
                if (tick)
                {
                    tick = 0;
                    /* Host can send double SET_INTERFACE request. */
                    if (0u != USB_IsConfigurationChanged())
                    {
                        usb_configure();
                    }
                    if (CTRLR_SCB.status == USB_XFER_STATUS_ACK)
                    {
                        process_msg((void*)CTRLR_INBOX);
                        CTRLR_SCB.status = USB_XFER_IDLE;
                    }
                    if (KBD_SCB.status == USB_XFER_STATUS_ACK)
                    {
                        led_status = KBD_INBOX[0];
                        KBD_SCB.status = USB_XFER_IDLE;
                    }
                    if (status_register.matrix_output > 0)
                        report_matrix_readouts();
                    pipeline_process();
                }
                // Timer ISR will wake us up.
                CyPmAltAct(PM_ALT_ACT_TIME_NONE, PM_ALT_ACT_SRC_NONE);
                break;
            case DEVSTATE_WATCH:
                if (tick > SUSPEND_SYSTIMER_DIVISOR)
                {
                    tick = 0;
                    scan_start();
                    if (pipeline_process_wakeup())
                    {
                        usb_wakeup();
                    }
                }
                CyPmAltAct(PM_ALT_ACT_TIME_NONE, PM_ALT_ACT_SRC_NONE);
                break;
            case DEVSTATE_RESUMING:
                wake();
                break;
            case DEVSTATE_SUSPENDING:
                nap();
                break;
            default:
                // Stray interrupt? We'd better stay awake.
                break;
        }
    }
}

/* [] END OF FILE */
