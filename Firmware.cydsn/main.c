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

void Boot_Load(void)
{
    LED_Write(1u);
    CyDelayUs(20);
    LED_Write(0u);
}

void BootIRQ_Interrupt_InterruptCallback(void)
{
    Boot_Load();
}

uint8 matrix[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
uint32_t matrix_status[ABSOLUTE_MAX_ROWS];
uint32_t matrix_prev[ABSOLUTE_MAX_ROWS];

/*
const uint32_t Drives[] = {
    Rows0_0, Rows0_1, Rows0_2, Rows0_3, Rows0_4, Rows0_5, Rows0_6, Rows0_7
};

const uint32_t Senses[] = {
    Cols0_0, Cols0_1, Cols0_2, Cols0_3, Cols0_4, Cols0_5, Cols0_6, Cols0_7,
    Cols1_0, Cols1_1, Cols1_2, Cols1_3, Cols1_4, Cols1_5, Cols1_6, Cols1_7
};
*/

static uint8 Buf0Mem[10], Buf1Mem[10]; // Actual results are 16 bits, but we don't need that much. So we read half.

    // Last column - discharge everything we can, then add small amount (have to, trigger!) and remove.
/*
 * Discharge sensor before scanning. It picks up lots of noise between runs.
 */
void Discharge()
{
    SenseReg0_Write(0xff);
    SenseReg1_Write(0xff);
    DriveReg0_Write(0x80); 
    
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

void primeSensor(uint8 part)
{
    uint16 tt = 0b0101010101010101;
    SenseReg0_Write((tt << part) & 0xff);
    SenseReg1_Write((tt << part) & 0xff);
}

uint8_t maxlevel = 0;
uint32_t pings = 0;
uint32_t lpings = 0;

void scanColumn(uint8 col, uint8_t part)
{
    primeSensor(part);
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
    for(int i=part; i<8; i+=2) {
        // Since Count7 counts down, _last_ channel is first in buffer.
        // To save on computation here, channels in schematic are swapped - 6-4-2-0 instead of 0-2-4-6
        matrix[col][i] = Buf0Mem[i+2];
        matrix[col][i + 8] = Buf1Mem[i+2];
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
    Discharge();
    //FIXME rename everything - we're scanning rows and read columns actually!
    for(uint8 i = 0; i<8; i++){
        scanColumn(i, 0);
        scanColumn(i, 1);
//        scanColumn((i+4) % 8, 1);
    }
}

void scan(void)
{
    memset(matrix, 0xbf, ABSOLUTE_MAX_COLS*ABSOLUTE_MAX_ROWS);
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
static uint8 Buf0Chan, Buf1Chan;
static uint8 Buf0TD = CY_DMA_INVALID_TD;
static uint8 Buf1TD = CY_DMA_INVALID_TD;

void InitSensor(void)
{
    uint8 enableInterrupts;
    /* Init DMA, 2 bytes bursts, each burst requires a request */
    Buf0Chan = Buf0_DmaInitialize(1, 1, (uint16)(HI16(CYDEV_PERIPH_BASE)), (uint16)(HI16(CYDEV_SRAM_BASE)));
    Buf1Chan = Buf1_DmaInitialize(1, 1, (uint16)(HI16(CYDEV_PERIPH_BASE)), (uint16)(HI16(CYDEV_SRAM_BASE)));
    //(*(reg8 *)PTK_ChannelCounter__PERIOD_REG) = 3u;
    enableInterrupts = CyEnterCriticalSection();
    (*(reg8 *)PTK_ChannelCounter__CONTROL_AUX_CTL_REG) |= (uint8)0x20u; // Init count7
    CyExitCriticalSection(enableInterrupts);
    ADC0_Start();
    ADC1_Start();
    CyDelayUs(10); // Give ADCs time to warm up
}

void BufferSetup(uint8* chan, uint8* td, uint8 channel_config, uint32 src_addr, uint32 dst_addr)
{
    (void)CyDmaClearPendingDrq(*chan);
    if (*td == CY_DMA_INVALID_TD) *td = CyDmaTdAllocate();
    (void) CyDmaTdSetConfiguration(*td, (uint16)10u, *td, (channel_config | (uint8)TD_INC_DST_ADR));
    (void) CyDmaTdSetAddress(*td, LO16(src_addr), LO16(dst_addr));
    (void) CyDmaChSetInitialTd(*chan, *td);
    (void) CyDmaChEnable(*chan, 1);
}

void EnableSensor(void)
{
    BufferSetup(&Buf0Chan, &Buf0TD, Buf0__TD_TERMOUT_EN, (uint32)ADC0_ADC_SAR__WRK0, (uint32)Buf0Mem);
/*
    (void)CyDmaClearPendingDrq(Buf0Chan);
    if (Buf0TD == CY_DMA_INVALID_TD) Buf0TD = CyDmaTdAllocate();
    (void) CyDmaTdSetConfiguration(Buf0TD, 10u,  Buf0TD, ((uint8)Buf0__TD_TERMOUT_EN | (uint8)TD_INC_DST_ADR));
    (void) CyDmaTdSetAddress(Buf0TD, LO16((uint32)ADC0_ADC_SAR__WRK0), LO16((uint32)Buf0Mem));
    (void) CyDmaChSetInitialTd(Buf0Chan, Buf0TD);
    (void) CyDmaChEnable(Buf0Chan, 1);
    
    (void)CyDmaClearPendingDrq(Buf1Chan);
    if (Buf1TD == CY_DMA_INVALID_TD) Buf1TD = CyDmaTdAllocate();
    (void) CyDmaTdSetConfiguration(Buf1TD, 10u,  Buf1TD, ((uint8)Buf1__TD_TERMOUT_EN | (uint8)TD_INC_DST_ADR));
    (void) CyDmaTdSetAddress(Buf1TD, LO16((uint32)ADC1_ADC_SAR__WRK0), LO16((uint32)Buf1Mem));
    (void) CyDmaChSetInitialTd(Buf1Chan, Buf1TD);
    (void) CyDmaChEnable(Buf1Chan, 1);
*/
    BufferSetup(&Buf1Chan, &Buf1TD, Buf1__TD_TERMOUT_EN, (uint32)ADC1_ADC_SAR__WRK0, (uint32)Buf1Mem);
    (*(reg8 *)PTK_CtrlReg__CONTROL_REG) = (uint8)0b11u; // enable counter's clock, generate counter load pulse
}

void SetupSensor(void)
{
    InitSensor();
    EnableSensor();
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
    SetupSensor();
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
            for(uint8 i = 0; i<config.matrixCols; i++)
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
