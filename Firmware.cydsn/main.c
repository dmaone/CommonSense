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
    tick = 1;
    systime++;
    if (USB_CheckActivity())
    {
        usb_idletime++;
    }
    else
    {
        usb_idletime = 0;
    }
}

// power modes
// CyIMO_SetFreq(CY_IMO_FREQ_3MHZ);
// CyFlash_SetWaitCycles(3);
// CyDelayFreq(3000000); - not supposed to be used, busy loops are evil
// disable fast IMO on startup


int main()
{
    BootIRQ_StartEx(BootIRQ_ISR);
    CyGlobalIntEnable; /* Enable global interrupts. */
    load_config();
    status_register.matrix_output = 0;
    status_register.emergency_stop = 0;
    status_register.setup_mode = NOT_A_KEYBOARD;
    scan_init();
    scan_start(); // fill matrix state, check if ADC is operational
    CyDelay(10);
    USB_Start(0u, USB_5V_OPERATION);
    usb_init();
    SysTimer_WritePeriod(BCLK__BUS_CLK__KHZ); // Need 1kHz
    SysTimer_Start();
    TimerIRQ_StartEx(Timer_ISR);
    pipeline_init();
    for(;;)
    {
        if (tick)
        {
            tick = 0;
            /* Host can send double SET_INTERFACE request. */
            if (0u != USB_IsConfigurationChanged())
            {
                usb_init();
                usb_idletime = 0;
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
    }
}

/* [] END OF FILE */
