/*
 *
 * Copyright (C) 2016-2017 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
*/
#include <project.h>

#include "scan.h"

static uint8_t Buf0Chan, Buf1Chan;
static uint8_t Buf0TD = CY_DMA_INVALID_TD;
static uint8_t Buf1TD = CY_DMA_INVALID_TD;
// signedness intentional! Simplifies comparison logic
// high byte from ADC must be zero, so should be safe.
static int16_t Buf0Mem[PTK_CHANNELS], Buf1Mem[PTK_CHANNELS];
static uint8_t current_row;
static bool scan_in_progress;
uint32_t matrix_status[MATRIX_ROWS];
uint32_t row_status;

CY_ISR_PROTO(HWStateIRQ_ISR);

static void InitSensor(void)
{
    // Init DMA, each burst requires a request 
    Buf0Chan = Buf0_DmaInitialize(sizeof *Buf0Mem, 1, (uint16)(HI16(CYDEV_PERIPH_BASE)), (uint16)(HI16(CYDEV_SRAM_BASE)));
    Buf1Chan = Buf1_DmaInitialize(sizeof *Buf1Mem, 1, (uint16)(HI16(CYDEV_PERIPH_BASE)), (uint16)(HI16(CYDEV_SRAM_BASE)));
    uint8 enableInterrupts = CyEnterCriticalSection();
    (*(reg8 *)PTK_ChannelCounter__CONTROL_AUX_CTL_REG) |= (uint8)0x20u; // Init count7
    CyExitCriticalSection(enableInterrupts);
    ADC0_Start();
    ADC0_SetResolution(ADC_RESOLUTION);
    ADC1_Start();
    ADC1_SetResolution(ADC_RESOLUTION);
    CyDelayUs(10); // Give ADCs time to warm up
    ChargeDelay_Start();
    scancode_buffer_readpos = 0;
    scancode_buffer_writepos = 0;
}

static void BufferSetup(uint8* chan, uint8* td, uint8 channel_config, uint32 src_addr, uint32 dst_addr)
{
    (void)CyDmaClearPendingDrq(*chan);
    if (*td == CY_DMA_INVALID_TD) *td = CyDmaTdAllocate();
    // transferCount is actually bytes, not transactions.
    (void) CyDmaTdSetConfiguration(*td, (uint16)sizeof Buf0Mem, *td, (channel_config | (uint8)TD_INC_DST_ADR));
    (void) CyDmaTdSetAddress(*td, LO16(src_addr), LO16(dst_addr));
    (void) CyDmaChSetInitialTd(*chan, *td);
    (void) CyDmaChEnable(*chan, 1);
}

static void EnableSensor(void)
{
    BufferSetup(&Buf0Chan, &Buf0TD, Buf0__TD_TERMOUT_EN, (uint32)ADC0_ADC_SAR__WRK0, (uint32)Buf0Mem);
    BufferSetup(&Buf1Chan, &Buf1TD, Buf1__TD_TERMOUT_EN, (uint32)ADC1_ADC_SAR__WRK0, (uint32)Buf1Mem);
    (*(reg8 *)PTK_CtrlReg__CONTROL_REG) = (uint8)0b11u; // enable counter's clock, generate counter load pulse
    HWStateIRQ_StartEx(HWStateIRQ_ISR);
    HWState_InterruptEnable();
}

inline void Drive(uint8 drv)
{
    SenseReg_Control |= 0x01; // Untie sense from the ground
/*
 * CyPins_SetPinDriveMode is HELLISHLY expensive (about 1us/call)
 * So changing drive mode in realtime to enhance crosstalk immunity is _not_ a good idea.
 * reading the row in 4us is pointless if you spend 20us setting drive modes.
*/
    //SetPin should not be used because it doesn't trigger start circuitry
    DriveReg0_Write(1 << drv);
}

void scan_start(void)
{
    if (!scan_in_progress)
    {
        current_row = 0;
        Drive(current_row);
        scan_in_progress = true;
    }
}

inline void append_scancode(uint8_t scancode)
{
    scancode_buffer_writepos = (scancode_buffer_writepos + 1) % sizeof(scancode_buffer);
    scancode_buffer[scancode_buffer_writepos] = scancode;
}

inline void filter(uint8_t row, uint8_t col, int16_t incoming)
{
    if ((incoming <= config.deadBandLo[row][col] && incoming + config.guardLo >= config.deadBandLo[row][col]) // Lower band
     || (incoming >= config.deadBandHi[row][col] && incoming - config.guardHi <= config.deadBandHi[row][col]) // Upper band
    ) {
#if COMMONSENSE_IIR_ORDER == 0
        // Degenerate version. But very fast! Can be used in noiseless environments.
        matrix[row][col] = incoming;
#else
        matrix[row][col] += incoming - (matrix[row][col] >> COMMONSENSE_IIR_ORDER);
#endif
    }
    //Key pressed?
#if NORMALLY_LOW == 1
    if (matrix[row][col] >= (config.deadBandHi[row][col] << COMMONSENSE_IIR_ORDER))
#else
    if (matrix[row][col] <= (config.deadBandLo[row][col] << COMMONSENSE_IIR_ORDER))
#endif
    {
        if ((row_status & (1 << col)) == 0)
        {
            // new keypress
            append_scancode(row*MATRIX_COLS+col);
            row_status |= (1 << col);
        }
    }
    else
    {
        if (row_status & (1 << col))
        {
            // new key release
            append_scancode(0x80|(row*MATRIX_COLS+col));
            row_status &= ~(1 << col);
        }
    }
}

CY_ISR(HWStateIRQ_ISR)
{
    if (!scan_in_progress)
        return;
#ifdef COMMONSENSE_100KHZ_MODE
    Drive(0);
    return;
    // The rest of the code is dead in 100kHz mode.
#endif
    row_status = matrix_status[current_row];
    for(uint8_t i=0; i<ADC_CHANNELS; i++) {
        // When monitoring matrix we're interested in raw feed.
        if (status_register.matrix_output)
        {
            matrix[current_row][i]                  = Buf0Mem[_ADC_COL_OFFSET(i)];
            matrix[current_row][i + ADC_CHANNELS]   = Buf1Mem[_ADC_COL_OFFSET(i)];
        }
        else
        {
            filter(current_row, i, Buf0Mem[_ADC_COL_OFFSET(i)]);
            filter(current_row, i + ADC_CHANNELS, Buf1Mem[_ADC_COL_OFFSET(i)]);
        }
    }
    matrix_status[current_row] = row_status;
    uint8_t next_row = (current_row ? current_row : config.matrixRows) - 1;
    // Drive row. Here, because there's not enough time to read out the array (not reliably) if driven at the top.
    // There were before bandpass filter - and next_tow was defined right above the loop and made sense.
    Drive(next_row);
    current_row = next_row;
    HWState_Read(); // Clears IRQ request. Must come AFTER processing.
}


void scan_init(void)
{
    InitSensor();
    EnableSensor();
}

//TODO move to scancode processor module.
void process_scancode_buffer(void)
{
    if (scancode_buffer_readpos == scancode_buffer_writepos)
        return;
}
