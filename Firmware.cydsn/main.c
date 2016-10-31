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

const uint32_t Drives[] = {
    Rows0_0, Rows0_1, Rows0_2, Rows0_3, Rows0_4, Rows0_5, Rows0_6, Rows0_7
};

const uint32_t Senses[] = {
    Cols0_0, Cols0_1, Cols0_2, Cols0_3, Cols0_4, Cols0_5, Cols0_6, Cols0_7,
    Cols1_0, Cols1_1, Cols1_2, Cols1_3, Cols1_4, Cols1_5, Cols1_6, Cols1_7
};

#define INTERLEAVE 2
//const uint16 SenseMap = 0b1001001001001001; //every 3rd
const uint16 SenseMap = 0b0101010101010101; //every 2nd

void SetColumns(uint8 col)
{
    CyPins_SetPinDriveMode(Drives[col], PIN_DM_RES_UPDWN);
    //SetPin should not be used because it doesn't trigger start circuitry
    DriveReg0_Write((1 << col) & 0xff);
}

void PinColumn(uint8 col)
{
    CyPins_SetPinDriveMode(Drives[col], PIN_DM_OD_LO);
}

void enableSensor(uint8 part)
{
    if (!config.capsense_flags.interlacedScan)
    {
        SenseReg0_Write(0xff);
    } else {
        uint16_t sm = (SenseMap << part) & 0xffff;
        for(uint8 i=0; i<16; i++) {
            if ( sm & (1 << i) ) {
                CyPins_SetPinDriveMode(Senses[i], PIN_DM_RES_UPDWN);
            } else {
                CyPins_SetPinDriveMode(Senses[i], PIN_DM_OD_LO);
            }
        }
        SenseReg0_Write(sm & 0xff); // !!!TODO!! pattern!
        SenseReg1_Write((sm >> 8) & 0xff); // !!!TODO!! pattern!
    }
}

uint8_t maxlevel = 0;
uint32_t pings = 0;
uint32_t lpings = 0;

void scanColumn(uint8 col, uint8_t part)
{
    //CyDelayUs(3);
    enableSensor(part);
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
        matrix[col][INTERLEAVE*i + part] += ADC_GetResult16(INTERLEAVE*i + part) & 0xff;
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
    //FIXME rename everything - we're scanning rows and read columns actually!
    for(uint8 i = 0; i<8; i++) {
        scanColumn(i, 0);
        scanColumn((i+4)%8, 1);
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
