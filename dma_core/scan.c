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
static uint8_t Buf0Mem[PTK_CHANNELS], Buf1Mem[PTK_CHANNELS];
static uint8_t current_col;
static bool scan_in_progress;

CY_ISR(HWStateIRQ_ISR)
{
    HWState_Read();
    //HWStateIRQ_ClearPending();
    scan_update();
}

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

static void Drive(uint8 drv)
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
        current_col = config.matrixRows - 1;
        scan_in_progress = true;
        Drive(current_col);
    }
}

void scan_update(void)
{
    if (!scan_in_progress)
        return;
    
    for(int i=0; i<8; i++) {
        // Since Count7 counts down, _last_ channel is first in buffer.
        // To save on computation here, channels in schematic are swapped - 6-4-2-0 instead of 0-2-4-6
        // No filter
        //matrix[current_col][i] = Buf0Mem[i << 1];
        //matrix[current_col][i + 8] = Buf1Mem[i << 1];
        // IIR filter
        matrix[current_col][i] = (IIR_N * matrix[current_col][i] + Buf0Mem[i << 1]) >> IIR_M_BIT;
        matrix[current_col][i + 8] = (IIR_N * matrix[current_col][i + 8] + Buf1Mem[i << 1]) >> IIR_M_BIT;
    }
    if (current_col)
    {
        Drive(--current_col);
    }
    else
    {
        current_col = config.matrixRows - 1;
        // For 100kHz version, uncomment the line below and set PTK channels to 2 or 4
        // This will drive only line 0 and read 1 or 2 columns. But it will do that VERY fast.
        //current_col = 0;
        Drive(current_col);
    }
}

void scan_init(void)
{
    InitSensor();
    EnableSensor();
}
