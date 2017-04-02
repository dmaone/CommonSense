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
    usb_suspend_monitor_stop();
    uint8_t rwu = USB_RWUEnabled();
    USB_Suspend();
    if (rwu == 0)
    {
        power_state = DEVSTATE_SLEEP;
        CyPmAltAct(PM_ALT_ACT_TIME_NONE, PM_ALT_ACT_SRC_NONE);
    }
    else
    {
        power_state = DEVSTATE_WATCH;
        //SetFreq hangs us dry.
        //CyIMO_SetFreq(CY_IMO_FREQ_24MHZ);
// CyFlash_SetWaitCycles(3);
// CyDelayFreq(3000000); - not supposed to be used, busy loops are evil
    }
}

void wake(void)
{
    USB_Resume();
    power_state = DEVSTATE_WARMUP;
    //CyIMO_SetFreq(CY_IMO_FREQ_USB);
}

void full_on(void)
{
    power_state = DEVSTATE_FULL_THROTTLE;
    usb_suspend_monitor_start();
    scan_start();
}

int main()
{
    BootIRQ_StartEx(BootIRQ_ISR);
    CyGlobalIntEnable; /* Enable global interrupts. */
    load_config();
    SysTimer_WritePeriod(BCLK__BUS_CLK__KHZ); // Need 1kHz
    SysTimer_Start();
    TimerIRQ_StartEx(Timer_ISR);

    status_register.matrix_output = 0;
    status_register.emergency_stop = 0;
    status_register.setup_mode = NOT_A_KEYBOARD;
    usb_init();
    scan_init();
    pipeline_init(); // calls scan_reset
    scan_start(); // We are starting in full power - must do that initial kick
    usb_configure();
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
CyPins_SetPin(ExpHdr_2);
                    tick = 0;
                    //scan_start();
                    //if (pipeline_process_wakeup())
                    {
                    //    usb_wakeup();
                    }
CyPins_ClearPin(ExpHdr_2);
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
            case DEVSTATE_WARMUP:
                if (0u != USB_IsConfigurationChanged())
                {
                    usb_configure();
                }
                if (tick > SUSPEND_WARMUP_DELAY)
                {
                    full_on();
                }
                break;
            default:
                // Stray interrupt? We'd better stay awake.
                break;
        }
    }
}

/* [] END OF FILE */
