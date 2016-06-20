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
#include "usb_driver.h"
#include "c2/c2_protocol.h"
#include "c2/nvram.h"

void BootIRQ_Interrupt_InterruptCallback(void)
{
    Boot_Load();
}

uint8 matrix[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
uint32_t matrix_status[ABSOLUTE_MAX_ROWS];

uint32 rowpins[8] = {
    Rows_0, Rows_1, Rows_2, Rows_3, 
    Rows_4, Rows_5, Rows_6, Rows_7 
};
uint32 colpins[16] = {
    Columns0_0, Columns0_1, Columns0_2, Columns0_3, 
    Columns0_4, Columns0_5, Columns0_6, Columns0_7,
    Columns1_0, Columns1_1, Columns1_2, Columns1_3, 
    Columns1_4, Columns1_5, Columns1_6, Columns1_7 
};

void ResetColumns(uint8 col)
{
    Columns0_Write(0);
    Columns1_Write(0);
}

void enableSensor(uint8 half)
{
    if (!config.capsense_flags.interlacedScan)
    {
        InputControl_Write(0u);
    } else {
        // Selectively untie rows from the ground.
        for(uint8 i=half; i<config.matrixRows; i+=2)
            CyPins_SetPin(rowpins[i]);
    }
}

void dischargeSensor(uint8 half)
{
    if (!config.capsense_flags.interlacedScan)
    {
        InputControl_Write(1u);
    } else {
        for(uint8 i=half; i<config.matrixRows; i+=2)
            // Pull rows back down
            CyPins_ClearPin(rowpins[i]);
    }
}

void scanColumn(uint8 col, uint8_t half)
{
    enableSensor(half);
    CyPins_SetPin(colpins[col]);
    ADC_StartConvert();
    ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
    dischargeSensor(half);
    //CyDelay(5);
    ResetColumns(col);
    for(uint8 i=0; i<config.matrixRows; i++) {
        if (config.capsense_flags.interlacedScan && (i % 2 != half))
            continue;
        matrix[i][col] += ADC_GetResult16(i) & 0xff;
    }
}

void printColumn(uint8 col)
{
    outbox.response_type = C2RESPONSE_MATRIX_STATUS;
    outbox.payload[0] = col;
    outbox.payload[1] = status_register.matrix_output;
    outbox.payload[2] = config.matrixRows;
    for(uint8 i=0; i<config.matrixRows; i++)
    {
        outbox.payload[i+3] = matrix[i][col]; 
    }
    usb_send(OUTBOX_EP);
}

void read_matrix(void)
{
    for(uint8 i = 0; i<config.matrixCols; ++i) {
        if (config.capsense_flags.interlacedScan)
            scanColumn(i, 0);
        scanColumn(i, 1);
    }
}

void scan(void)
{
    memset(matrix, 0x00, ABSOLUTE_MAX_COLS*ABSOLUTE_MAX_ROWS);
    read_matrix();
//    CyDelay(10);
    read_matrix();
    read_matrix();
    read_matrix();
    for(uint8 i = 0; i<config.matrixCols; ++i) {
        if (status_register.matrix_output > 0)
            printColumn(i);
    }
    //CyDelay(50);
}

bool is_matrix_changed(void)
{
    bool retval = false;
    ///FIXME use actual calibration
    for (uint8_t i=0; i<config.matrixRows; i++)
    {
        uint32_t current_row = matrix_status[i];
        for (uint8_t j=0; j<config.matrixCols; j++)
        {
            if (config.storage[STORAGE_ADDRESS((i*config.matrixCols)+j)] > 3u) {
                if (matrix[i][j] > config.thresholdVoltage)
                    current_row |= (1 << j);
                else
                    current_row &= ~(1 << j);
            }
        }
        if (current_row != matrix_status[i]) {
#if 1 == 0
            xprintf("Row %d keys %08x -> %08x", i, matrix_status[i], current_row);
            for (uint8_t j=0; j<config.matrixCols; j++)
            {
                if (matrix[i][j] > config.thresholdVoltage)
                    xprintf("Keypress: %d %d %d", i, j, matrix[i][j]);
            }
#endif
            matrix_status[i] = current_row;
            retval = true;
        }
    }
    return retval;
}

uint8_t prev_report[64];
uint8_t this_report[64];

void send_report(void)
{
    // Our report is 1b modifiers 62b other keys. No LEDs.
    uint8_t count = 1; // start from second byte
    memset(this_report, 0x00, 64);
    if (!status_register.emergency_stop)
    {
        for (uint8_t i=0; i<config.matrixRows; i++)
        {
            for (uint8_t j=0; j<config.matrixCols; j++)
            {
                if (config.col_params->isActive && (matrix_status[i] & (1 << j)))
                {
                    // Keypress!
                    uint8_t keycode = config.storage[STORAGE_ADDRESS((i*config.matrixCols)+j)];
                    if ((keycode & 0xF8) == 0xE0) 
                    {
                        // modifier. set the modifier bit.
                        this_report[0] |= (1 << (keycode & 7));
                    } else if (keycode > 0x03) {
                        this_report[count++] = keycode;
                    }
                    xprintf("KP: %d %d %x", i, j, keycode);
                }
            }
        }
    }
    if (memcmp(this_report, prev_report, 64) != 0) {
        xprintf("P/T %x %x %x %x, %d", prev_report[0], prev_report[1], this_report[0], this_report[1], count);
        memcpy(prev_report, this_report, 64);
        // Send actual report - per-key calibration still needed.
        memcpy(outbox.raw, this_report, 64);
        usb_send(KEYBOARD_EP);
    }
}

int main()
{
    for(uint8 i=0; i<5;i++){
        LED_Write(1u);
        CyDelay(5u);
        LED_Write(0u);
        CyDelay(100u);
    }
    BootIRQ_Start();
    CyGlobalIntEnable; /* Enable global interrupts. */
    load_config();
    status_register.matrix_output = 0;
    status_register.emergency_stop = 0;
    ADC_Start();
    ADC_SetResolution(10u);
    USB_Start(0u, USB_5V_OPERATION);
    usb_init();
    if (config.thresholdVoltage < 1)
        status_register.emergency_stop = true;
    xprintf("Start");
    memset(prev_report, 0x00, sizeof(outbox));
    scan(); // fill matrix state
    for(;;)
    {
        /* Host can send double SET_INTERFACE request. */
        if (0u != USB_IsConfigurationChanged())
        {
            usb_init();
        }
        if (message_for_you_in_the_lobby)
        {
            process_msg();
        }
        scan();
        if (is_matrix_changed()) {
            send_report();
        }
    }
}

/* [] END OF FILE */
