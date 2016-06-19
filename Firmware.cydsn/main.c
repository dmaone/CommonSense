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
#include "c2/nvram.h"
#include "print.h"

void BootIRQ_Interrupt_InterruptCallback(void)
{
    Boot_Load();
}

uint8 matrix[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];

uint32 rowpins[8] = {
    Rows_0, Rows_1, Rows_2, Rows_3, 
    Rows_4, Rows_5, Rows_6, Rows_7 
};

uint8 current_column;

void SetColumns(uint8 col)
{
    //TODO use per-pin API
    if (col < 8u)
    {
        Columns0_Write(1 << col);
    }
    else
    {
        Columns1_Write(1 << (col - 8));
    }
}

void ResetColumns(uint8 col)
{
    Columns0_Write(0);
    Columns1_Write(0);
}

// performs actual column read.
void senseColumn(uint8_t col)
{
    InputControl_Write(0u);
    SetColumns(col);
//    CyDelayUs(250);
    ADC_StartConvert();
    ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
    InputControl_Write(1u);
    ResetColumns(col);
//    CyDelayUs(250);
}

void setupSensor(uint8 half)
{
    // Grounding columns we WILL NOT use.
    for(uint8 i=0; i<config.matrixRows; i++)
        if (i % 2 == half)
            CyPins_ClearPin(rowpins[i]);            
        else
            CyPins_SetPin(rowpins[i]);
}

void ScanColumn(uint8 col, uint8_t half)
{
    senseColumn(col);
    for(uint8 i=0; i<config.matrixRows; i++) {
        if (config.capsense_flags.interlacedScan && (i % 2 == half))
            continue;
        matrix[i][col] = ADC_GetResult16(i) & 0xff;
    }
}

void PrintColumn(uint8 col)
{
    if (status_register.matrix_output > 0)
    {
        outbox.response_type = C2RESPONSE_MATRIX_STATUS;
        outbox.payload[0] = col;
        outbox.payload[1] = status_register.matrix_output;
        outbox.payload[2] = config.matrixRows;
        for(uint8 i=0; i<config.matrixRows; i++)
        {
            outbox.payload[i+3] = matrix[i][col]; 
        }
        usb_send();
    }
}

void scanPass(uint8 half)
{
    setupSensor(half);
    for(uint8 i = 0; i<config.matrixCols; ++i)
    {
        ScanColumn(i, half);
        if (half) // Last pass
            PrintColumn(i);
        
    }
}

void scan(void)
{
    if (config.capsense_flags.interlacedScan)
        scanPass(0);
    scanPass(1);
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
    load_config();
    usb_init();
    xprintf("Start");
    ADC_Start();
    ADC_SetResolution(10u);
    status_register.matrix_output = 0;
    status_register.emergency_stop = 0;
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
        if (message_for_you_in_the_lobby)
        {
            process_msg();
        }
        scan();
    }
}

/* [] END OF FILE */
