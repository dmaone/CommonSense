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
#include "cyapicallbacks.h"

void BootIRQ_Interrupt_InterruptCallback(void)
{
    Boot_Load();
}

uint8 matrix[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
uint32_t matrix_status[ABSOLUTE_MAX_ROWS];
uint32_t matrix_prev[ABSOLUTE_MAX_ROWS];
const uint32_t Drives[] = {Drive0_0, Drive0_1, Drive0_2, Drive0_3, Drive0_4, Drive0_5, Drive0_6, Drive0_7};
const uint32_t Senses[] = {
    OddSense_0, EvenSense_0, OddSense_1, EvenSense_1, OddSense_2, EvenSense_2, OddSense_3, EvenSense_3,
    OddSense_4, EvenSense_4, OddSense_5, EvenSense_5, OddSense_6, EvenSense_6, OddSense_7, EvenSense_7
};

void SetColumns(uint8 col)
{
    // Prevent premature triggering    
    CyPins_SetPinDriveMode(Drives[col], PIN_DM_RES_UPDWN);
    //CyPins_SetPin(Drives[col]);
    ColReg0_Write((1 << col) & 0xff);
//    ColReg1_Write(0u);
//    ColReg1_Write(1 << (col - 8));
}

void PinColumn(uint8 col)
{
    CyPins_SetPinDriveMode(Drives[col], PIN_DM_OD_LO);
}

void enableSensor(uint8 half)
{
    if (!config.capsense_flags.interlacedScan)
    {
        SenseControl_Write(0xff);
    } else {
        for(uint8 i=0; i<16; i++) {
            if (i % 2 == half) {
                CyPins_SetPinDriveMode(Senses[i], PIN_DM_RES_UPDWN);
            } else {
                CyPins_SetPinDriveMode(Senses[i], PIN_DM_OD_LO);
            }
        }
        //SenseControl_Write(0xff);
        SenseControl_Write(half+1);
    }
}

uint8_t maxlevel = 0;
uint32_t pings = 0;
uint32_t lpings = 0;

void scanColumn(uint8 col, uint8_t half)
{
    CyDelayUs(5);
    enableSensor(half);
    SetColumns(col);
    uint8_t cnt = 255;
    while (cnt && !(HWState_Read() & 0x01))
    {
        CyDelayUs(1);
        cnt--;
    }// Wait for HW
    if (!cnt) {
        // Ugly hack. Timeout here means ADC didn't start properly.
        // So far this only happened on startup - if it starts normally, it works.
        // Probability of not starting seems about 20%. So 2-3 
        //CySoftwareReset();
        Boot_Load();

    }
    PinColumn(col);
    for(uint8 i=0; i<8; i++) {
//    for(uint8 i=0; i<config.matrixCols; i++) {
//        if (config.capsense_flags.interlacedScan && (i % 2 != half))
//            continue;
//        matrix[col][i] = ADC_Samples[i] & 0xff;
//        matrix[col][i] += ADC_GetResult16(i) & 0xff;
        matrix[col][(i * 2)+1 - half] += ADC_GetResult16((half * 8) + i) & 0xff;
//        if (maxlevel < matrix[col][i])
//            maxlevel = matrix[col][i];
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
    //uint8_t cur_col = 0;
    //uint8_t max_col = config.matrixCols + 1;
    //for(uint8 i = 0; i<config.matrixCols; ++i) {
    //FIXME rename everything - we're scanning rows and read columns actually!
    for(uint8 i = 0; i<8; i++) {
        //if (config.capsense_flags.interlacedScan)
            // shift 1 port on the column pins to reduce pin load.
            //scanColumn(( i + 8) % config.matrixCols, 0);
            //scanColumn((cur_col+8) % config.matrixCols, 0);
            scanColumn(i, 0);
            //CyDelayUs(50);
        scanColumn(i, 1);
        //scanColumn(cur_col, 1);
        //cur_col++;
        //cur_col = (cur_col + 3) % config.matrixCols;
        //CyDelayUs(50);
        //CyDelayUs(5);
  }
}

void scan(void)
{
    memset(matrix, 0x00, ABSOLUTE_MAX_COLS*ABSOLUTE_MAX_ROWS);
    read_matrix();
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
                {
                    current_row |= (1 << j);
                    //pings++;
                }
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
                    if ((matrix_prev[i] & (1 << j)) == 0) {
                        //xprintf("KP: %d %d %d", i, j, matrix[i][j]);
                    }
                }
            }
        }
    }
    memcpy(matrix_prev, matrix_status, sizeof(matrix_prev));
    if (memcmp(this_report, prev_report, 64) != 0) {
        //xprintf("P/T %x %x %x %x, %d", prev_report[0], prev_report[1], this_report[0], this_report[1], count);
        memcpy(prev_report, this_report, 64);
        // Send actual report - per-key calibration still needed.
        memcpy(outbox.raw, this_report, 64);
        //usb_send(KEYBOARD_EP);
    }
}

int main()
{
    LED_Write(1u);
    BootIRQ_Start();
    CyGlobalIntEnable; /* Enable global interrupts. */
    load_config();
    status_register.matrix_output = 0;
    status_register.emergency_stop = 0;
    RampPWM_Start();
    ADC_Start();
    scan(); // fill matrix state, check if ADC is operational
    
    USB_Start(0u, USB_5V_OPERATION);
    usb_init();
    LED_Write(0u);
    if (config.thresholdVoltage < 1)
        status_register.emergency_stop = true;
    memset(prev_report, 0x00, sizeof(outbox));
    KeyboardTimer_Start();
    //ChanDecoder_WritePeriod(config.matrixRows-1);
    uint32_t count = 0;
    uint32_t prev_timer = KeyboardTimer_ReadCounter();
    uint32_t now_timer = prev_timer;
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
        if (status_register.matrix_output > 0)
            for(uint8 i = 0; i<config.matrixCols; ++i)
                printColumn(i);
        if (is_matrix_changed())
        {
            //pings++;
            send_report();
        }
/*
        if (count++ % 1000 == 0)
        {
            prev_timer = now_timer;
            now_timer = KeyboardTimer_ReadCounter();
            xprintf("ms per 1k iterations: %d, pings: %u/%u, max level: %u", (prev_timer - now_timer), pings, lpings, maxlevel);
            pings = 0;
            maxlevel = 0;
            lpings = 0;
            //xprintf("%d", CyDmaChGetRequest(Buf0_Chan));
            //xprintf("%d", (uint16) ADC1_SAR_WRK0_REG);
            //xprintf("%d", ADC_Samples[0]);//, ADC_Samples[8]);
//            xprintf("%d", ADC_Samples[15]);
        }
*/
    }
}

/* [] END OF FILE */
