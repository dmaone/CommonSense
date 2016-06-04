/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
*/
#include <project.h>
#include "globals.h"
#include "keyboard.h"
#include "usb_driver.h"
#include "c2/c2_protocol.h"
#include "print.h"

void BootIRQ_Interrupt_InterruptCallback(void)
{
    Boot_Load();
}


uint8 current_column;

void SetColumns(uint8 col)
{
    current_column = col;
    if (current_column < 8u)
    {
        Columns_0_Write(1 << col);
    }
    else
    {
        Columns_1_Write(1 << (col - 8));
    }
}

void ResetColumns()
{
    if (current_column < 8u)
    {
        Columns_0_Write(0);
    }
    else
    {
        Columns_1_Write(0);
    }
}

void ScanColumn(uint8 col)
{
    InputControl_Write(0u);
//    CyDelayUs(10);
    SetColumns(col);
    //CyDelayUs(10);
    ADC_StartConvert();
    ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
    InputControl_Write(1u);
    ResetColumns();
}

void PrintColumn(uint8 col)
{
    uint16 res[8];
    for(uint8 i=0; i<8; ++i)
    {
        res[i] = ADC_GetResult16(i);
    }

    LED_Write(1u);
    if (ephemeral_debug.matrix_output > 0)
    {
        outbox.response_type = C2RESPONSE_MATRIX_STATUS;
        outbox.payload[0] = col;
        outbox.payload[1] = ephemeral_debug.matrix_output;
        memcpy(&outbox.payload[2], res, sizeof(res));
        usb_send();
    } else {
        CyDelay(5u);
    }
    LED_Write(0u);
    CyDelay(5u);
}

void scan(void)
{
    for(uint8 i = 0; i<16; ++i)
    {
        ScanColumn(i);
        PrintColumn(i);
    }
}

int main()
{
    BootIRQ_Start();
    CyGlobalIntEnable; /* Enable global interrupts. */
    for(uint8 i=0; i<5;i++){
        LED_Write(1u);
        CyDelay(10u);
        LED_Write(0u);
        CyDelay(100u);
    }
    usb_init();
    LED_Write(0u);
    ADC_Start();
    ephemeral_debug.matrix_output = 0;
    //xprintf("Start");
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    for(;;)
    {
        /* Host can send double SET_INTERFACE request. */
    //    if (0u != USB_IsConfigurationChanged())
    //    {
            /* Initialize IN endpoints when device is configured. */
    //        if (0u != USB_GetConfiguration())
     //       {
                /* Enumeration is done, enable OUT endpoint to receive data 
                 * from host. */
     //           USB_EnableOutEP(8);
     //           xprintf("Reconfigured");
     //       }
     //   }
     //  xprintf("Start");
        if (message_for_you_in_the_lobby)
        {
            // This one is working @160602
            process_msg();
        }
        scan();
        //CyDelay(200);
        //LED_Write(0u);
        //CyDelay(200);
    }
}

/* [] END OF FILE */
