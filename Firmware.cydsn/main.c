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
//#include "cyfitter.h" // for MAGIC

void BootIRQ_Interrupt_InterruptCallback(void)
{
    Boot_Load();
}

#define NOT_A_KEYBOARD 1
// Should be number of columns + 2 (0 is ground, scan sequence is ch1-ch0-ch2-ch0-ch3-ch0..)
#define PTK_CHANNELS 18
// DO NOT FORGET TO UPDATE PTK COMPONENT!
#define CH0_OFFSET 0
int16 matrix[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
uint32_t matrix_status[ABSOLUTE_MAX_ROWS];
uint32_t matrix_prev[ABSOLUTE_MAX_ROWS];


static uint8 Buf0Chan, Buf1Chan;
static uint8 Buf0TD = CY_DMA_INVALID_TD;
static uint8 Buf1TD = CY_DMA_INVALID_TD;
static int16 Buf0Mem[PTK_CHANNELS], Buf1Mem[PTK_CHANNELS];

void InitSensor(void)
{
    /* Init DMA, 2 bytes bursts, each burst requires a request */
    Buf0Chan = Buf0_DmaInitialize(sizeof(Buf0Mem[1]), 1, (uint16)(HI16(CYDEV_PERIPH_BASE)), (uint16)(HI16(CYDEV_SRAM_BASE)));
    Buf1Chan = Buf1_DmaInitialize(sizeof(Buf1Mem[1]), 1, (uint16)(HI16(CYDEV_PERIPH_BASE)), (uint16)(HI16(CYDEV_SRAM_BASE)));
    uint8 enableInterrupts = CyEnterCriticalSection();
    (*(reg8 *)PTK_ChannelCounter__CONTROL_AUX_CTL_REG) |= (uint8)0x20u; // Init count7
    CyExitCriticalSection(enableInterrupts);
    ADC0_Start();
    ADC0_SetResolution(ADC_RESOLUTION);
    ADC1_Start();
    ADC1_SetResolution(ADC_RESOLUTION);
    CyDelayUs(10); // Give ADCs time to warm up
}

void BufferSetup(uint8* chan, uint8* td, uint8 channel_config, uint32 src_addr, uint32 dst_addr)
{
    (void)CyDmaClearPendingDrq(*chan);
    if (*td == CY_DMA_INVALID_TD) *td = CyDmaTdAllocate();
    // transferCount is actually bytes, not transactions.
    (void) CyDmaTdSetConfiguration(*td, (uint16)PTK_CHANNELS<<1, *td, (channel_config | (uint8)TD_INC_DST_ADR));
    (void) CyDmaTdSetAddress(*td, LO16(src_addr), LO16(dst_addr));
    (void) CyDmaChSetInitialTd(*chan, *td);
    (void) CyDmaChEnable(*chan, 1);
}

void EnableSensor(void)
{
    BufferSetup(&Buf0Chan, &Buf0TD, Buf0__TD_TERMOUT_EN, (uint32)ADC0_ADC_SAR__WRK0, (uint32)Buf0Mem);
    BufferSetup(&Buf1Chan, &Buf1TD, Buf1__TD_TERMOUT_EN, (uint32)ADC1_ADC_SAR__WRK0, (uint32)Buf1Mem);
    (*(reg8 *)PTK_CtrlReg__CONTROL_REG) = (uint8)0b11u; // enable counter's clock, generate counter load pulse
}

void Drive(uint8 drv)
{
/*
 * CyPins_SetPinDriveMode is HELLISHLY expensive (about 1us/call)
 * So changing drive mode in realtime to enhance crosstalk immunity is _not_ a good idea.
 * reading the row in 4us is pointless if you spend 20us setting drive modes.
*/
    //SetPin should not be used because it doesn't trigger start circuitry
    DriveReg0_Write(1 << drv);
}

uint8_t maxlevel = 0;
uint32_t pings = 0;
uint32_t lpings = 0;

void scanColumn(uint8 col, uint8_t part)
{
    SenseReg_Control |= 0x01; // Untie sense from the ground
    Drive(col);
    uint8_t cnt = 50;
    while (cnt && !(HWState_Read() & 0x01))
    {
        CyDelayUs(1);
        cnt--;
    }// Wait for HW
    if (!cnt) {
        // Something _very_ wrong happened.
        Boot_Load();

    }
    for(int i=0; i<8; i++) {
        // Since Count7 counts down, _last_ channel is first in buffer.
        // To save on computation here, channels in schematic are swapped - 6-4-2-0 instead of 0-2-4-6
        // No filter
        //matrix[col][i] = Buf0Mem[(i << 1) + CH0_OFFSET] - ADC_ZERO;
        //matrix[col][i + 8] = Buf1Mem[(i << 1) + CH0_OFFSET] - ADC_ZERO;
        // IIR filter
        matrix[col][i] = (3 * matrix[col][i] + Buf0Mem[(i << 1) + CH0_OFFSET] - ADC_ZERO) >> 2;
        matrix[col][i + 8] = (3 * matrix[col][i + 8] + Buf1Mem[(i << 1) + CH0_OFFSET] - ADC_ZERO) >> 2;
    }
}

void printRow(uint8 row)
{
    outbox.response_type = C2RESPONSE_MATRIX_STATUS;
    outbox.payload[0] = row;
    outbox.payload[1] = config.matrixCols;
    //int16 buf = 0;
    for(uint8 i=0; i<config.matrixCols; i++)
    {
        memcpy(outbox.payload + 2 + (i<<1), &matrix[row][i], 2);
    }
    usb_send(OUTBOX_EP);
}

void scan(void)
{
    //memset(matrix, 0xbf, ABSOLUTE_MAX_COLS*ABSOLUTE_MAX_ROWS);
    for(uint8 i = 0; i<8; i++){
        //scanColumn((i+4) % 8, 0);
        scanColumn(i, 0); // rotate 4 bits
        //scanColumn(i, 1);
    }
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
            if (config.storage[STORAGE_ADDRESS((i*config.matrixCols) + j)] > 3u) {
                // Not a dead key in layout
                if (matrix[i][j] > config.storage[i*config.matrixCols + j])
                {
                    // Above noise floor
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
                        xprintf("KP: %d %d %d", i, j, matrix[i][j]);
                    }
            }
        }
    }
    memcpy(matrix_prev, matrix_status, sizeof(matrix_prev));
    if (memcmp(this_report, prev_report, 64) != 0) {
        //xprintf("P/T %x %x %x %x %x, %d", prev_report[0], prev_report[1], this_report[0], this_report[1], this_report[2], count);
        memcpy(prev_report, this_report, 64);
        // Send actual report - per-key calibration still needed.
        memcpy(outbox.raw, this_report, 64);
        if (!status_register.emergency_stop)
        {
            usb_send(KEYBOARD_EP);
        }
    }
}
int main()
{
    LED_Write(1u);
    BootIRQ_Start();
    CyGlobalIntEnable; /* Enable global interrupts. */
    load_config();
    status_register.matrix_output = 0;
    status_register.emergency_stop = NOT_A_KEYBOARD;
    InitSensor();
    EnableSensor();
    scan(); // fill matrix state, check if ADC is operational
    USB_Start(0u, USB_5V_OPERATION);
    usb_init();
    LED_Write(0u);
    if (config.thresholdVoltage < 1)
        status_register.emergency_stop = true;
    memset(prev_report, 0x00, sizeof(outbox));
    KeyboardTimer_Start();
    //uint32_t count = 0;
    //uint32_t prev_timer = KeyboardTimer_ReadCounter();
    //uint32_t now_timer = prev_timer;
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
            for(uint8 i = 0; i<config.matrixRows; i++)
                printRow(i);
                
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
