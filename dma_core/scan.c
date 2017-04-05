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
#include "PSoC_USB.h"

CY_ISR_PROTO(EoC_ISR);
CY_ISR_PROTO(Result_ISR);

static uint8_t Buf0TD = CY_DMA_INVALID_TD;
static uint8_t Buf1TD = CY_DMA_INVALID_TD;
static uint8_t FinalBufTD[2];
// signedness intentional! Simplifies comparison logic
// high byte from ADC must be zero, so should be safe.
static int16_t BufMem[PTK_CHANNELS * NUM_ADCs];
static int16_t Results[ADC_CHANNELS * 2 * NUM_ADCs];
static uint8 driving_row, reading_row;
static bool scan_in_progress;
static uint32_t matrix_status[MATRIX_ROWS];

static uint16 matrix[MATRIX_ROWS][MATRIX_COLS];
static uint16 *matrix_ptr = (uint16 *)&matrix;

static void InitSensor(void)
{
    // Init DMA, each burst requires a request 
    Buf0_DmaInitialize(sizeof *BufMem, 1, (uint16)(HI16(CYDEV_PERIPH_BASE)), (uint16)(HI16(CYDEV_SRAM_BASE)));
    Buf1_DmaInitialize(sizeof *BufMem, 1, (uint16)(HI16(CYDEV_PERIPH_BASE)), (uint16)(HI16(CYDEV_SRAM_BASE)));
    FinalBuf_DmaInitialize(sizeof *Results * ADC_CHANNELS * 2, 2, (uint16)(HI16(CYDEV_SRAM_BASE)), (uint16)(HI16(CYDEV_SRAM_BASE)));
    uint8 enableInterrupts = CyEnterCriticalSection();
    (*(reg8 *)PTK_ChannelCounter__CONTROL_AUX_CTL_REG) |= (uint8)0x20u; // Init count7
    CyExitCriticalSection(enableInterrupts);
    ADC0_Start();
    ADC1_Start();
    ADC0_SetResolution(ADC_RESOLUTION);
    ADC1_SetResolution(ADC_RESOLUTION);
    ChargeDelay_Start();
    scan_reset();
}

static void BufferSetup(uint8 chan, uint8* td, uint8 channel_config, uint32 src_addr, uint32 dst_addr)
{
    (void)CyDmaClearPendingDrq(chan);
    if (*td == CY_DMA_INVALID_TD) *td = CyDmaTdAllocate();
    // transferCount is actually bytes, not transactions.
    (void) CyDmaTdSetConfiguration(*td, (uint16)ADC_BUFFER_BYTESIZE, *td, (channel_config | (uint8)TD_INC_DST_ADR));
    (void) CyDmaTdSetAddress(*td, LO16(src_addr), LO16(dst_addr));
    (void) CyDmaChSetInitialTd(chan, *td);
    (void) CyDmaChEnable(chan, 1);
}

static void ResultBufferSetup(void)
{
    CyDmaClearPendingDrq(FinalBuf_DmaHandle);
    FinalBufTD[0] = CyDmaTdAllocate();
    FinalBufTD[1] = CyDmaTdAllocate();
    // transferCount is actually bytes, not transactions.
    CyDmaTdSetConfiguration(FinalBufTD[0], (uint16)(sizeof Results / NUM_ADCs), FinalBufTD[1], CY_DMA_TD_INC_SRC_ADR | CY_DMA_TD_INC_DST_ADR | CY_DMA_TD_AUTO_EXEC_NEXT);
    CyDmaTdSetAddress(FinalBufTD[0], LO16((uint32)&BufMem[ADC_BUF_INITIAL_OFFSET]), LO16((uint32)&Results));
    CyDmaTdSetConfiguration(FinalBufTD[1], (uint16)(sizeof Results / NUM_ADCs), FinalBufTD[0], CY_DMA_TD_INC_SRC_ADR | CY_DMA_TD_INC_DST_ADR | FinalBuf__TD_TERMOUT_EN);
    CyDmaTdSetAddress(FinalBufTD[1], LO16((uint32)&BufMem[PTK_CHANNELS + ADC_BUF_INITIAL_OFFSET]), LO16((uint32)&Results[ADC_CHANNELS * 2]));
    CyDmaChSetInitialTd(FinalBuf_DmaHandle, FinalBufTD[0]);
    CyDmaChEnable(FinalBuf_DmaHandle, 1);
}

static void EnableSensor(void)
{
    BufferSetup(Buf0_DmaHandle, &Buf0TD, Buf0__TD_TERMOUT_EN, (uint32)ADC0_ADC_SAR__WRK0, (uint32)BufMem);
    BufferSetup(Buf1_DmaHandle, &Buf1TD, Buf1__TD_TERMOUT_EN, (uint32)ADC1_ADC_SAR__WRK0, ADC_BUFFER_BYTESIZE + (uint32)BufMem);
    ResultBufferSetup();
    (*(reg8 *)PTK_CtrlReg__CONTROL_REG) = (uint8)0b11u; // enable counter's clock, generate counter load pulse
    // It is important that ResultIRQ priority is less (=is higher)
    // so we don't start fetching next buffer before we are done with the current one.
    ResultIRQ_StartEx(Result_ISR);
    EoCIRQ_StartEx(EoC_ISR);
}

static inline void Drive(uint8 drv)
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

inline void append_scancode(uint8_t scancode)
{
    if (status_register.emergency_stop)
        return;
    scancode_buffer_writepos = SCANCODE_BUFFER_NEXT(scancode_buffer_writepos);
    scancode_buffer[scancode_buffer_writepos] = scancode;
}

CY_ISR(EoC_ISR)
{
    // If there's no scan in progress - one row will be filled by garbage.
    // Which is no big deal.
#ifdef COMMONSENSE_100KHZ_MODE
    Drive(0);
    return;
    // The rest of the code is dead in 100kHz mode.
#endif
    CyDmaChSetRequest(FinalBuf_DmaHandle, CY_DMA_CPU_REQ);
    uint8_t enableInterrupts = CyEnterCriticalSection();
    reading_row = driving_row;
    if (driving_row == 0)
    {
        // End of the scan pass. Loop if full throttle, otherwise stop.
        if (power_state != DEVSTATE_FULL_THROTTLE)
        {
            scan_in_progress = false;
            return;
        }
        driving_row = MATRIX_ROWS;
    }
    driving_row--;
    // Drive row.
    // DMA channel reading out results has priority, so this should not overwrite the results buffer.
    Drive(driving_row);
    CyExitCriticalSection(enableInterrupts);
}

CY_ISR(Result_ISR)
{
#ifdef COMMONSENSE_100KHZ_MODE
    return;
    // The rest of the code is dead in 100kHz mode.
#endif
    uint32_t row_status = matrix_status[reading_row];
    register uint8_t current_col = ADC_CHANNELS * NUM_ADCs;
    register uint8_t adc_buffer_pos = ADC_CHANNELS * NUM_ADCs * 2;
    while (current_col > 0)
    {
        current_col--;
        adc_buffer_pos -= 2;
        // Here you need matrix-sized array of uint8!! matrix[][] won't do!!
        register uint8_t key_index = (uint32)&config.deadBandHi[reading_row][current_col] - (uint32)&config.deadBandHi;
        register uint8_t hi = config.deadBandHi[reading_row][current_col];
        register uint8_t lo = config.deadBandLo[reading_row][current_col];
        register int16_t readout = Results[adc_buffer_pos];
        if (status_register.matrix_output)
        {
            // When monitoring matrix we're interested in raw feed.
            matrix_ptr[key_index] = readout;
            continue;
        }
        else if (hi == 0)
        {
            continue;
        } else if (
            !(
                (readout <= lo && readout + config.guardLo >= lo) // Lower band
                || 
                (readout >= hi && readout - config.guardHi <= hi) // Upper band
            )
        )
        {
            continue;
        }
#if COMMONSENSE_IIR_ORDER == 0
        // Degenerate version. But very fast! Can be used in noiseless environments.
        matrix_ptr[key_index] = readout;
#else
        matrix_ptr[key_index] += readout - (matrix_ptr[key_index] >> COMMONSENSE_IIR_ORDER);
#endif
//Key pressed?
#if NORMALLY_LOW == 1
        if (matrix_ptr[key_index] >= (hi << COMMONSENSE_IIR_ORDER))
#else
        if (matrix_ptr[key_index] <= (lo << COMMONSENSE_IIR_ORDER))
#endif
        {
            if ((row_status & (1 << current_col)) == 0)
            {
        // new keypress
                append_scancode(key_index);
#ifdef MATRIX_LEVELS_DEBUG
                level_buffer[scancode_buffer_writepos] = matrix_ptr[key_index] & 0xff;
                level_buffer_inst[scancode_buffer_writepos] = readout & 0xff;
#endif
                row_status |= (1 << current_col);
            }
        }
        else
        {
            if ((row_status & (1 << current_col)) > 0)
            {
        // new key release
                append_scancode(KEY_UP_MASK|key_index);
#ifdef MATRIX_LEVELS_DEBUG
                level_buffer[scancode_buffer_writepos] = matrix_ptr[key_index] & 0xff;
                level_buffer_inst[scancode_buffer_writepos] = readout & 0xff;
#endif
                row_status &= ~(1 << current_col);
            }
        }
    }
    matrix_status[reading_row] = row_status;
}

void scan_start(void)
{
    if (!scan_in_progress)
    {
        driving_row = MATRIX_ROWS - 1; // Zero-based! Adjust!
        Drive(driving_row);
        scan_in_progress = true;
    }
}

void scan_reset(void)
{
    uint8_t enableInterrupts = CyEnterCriticalSection();
    for (uint8_t i=0; i<MATRIX_ROWS; i++)
    {
        for (uint8_t j=0; j<MATRIX_COLS; j++)
        {
            // Away from thresholds! Account for IIR.
            matrix[i][j] = (config.deadBandHi[i][j] + config.deadBandLo[i][j]) << (COMMONSENSE_IIR_ORDER - 1);
        }
    }
    memset(scancode_buffer, COMMONSENSE_NOKEY, sizeof(scancode_buffer));
    memset(matrix_status, 0, sizeof(matrix_status));
    scancode_buffer_readpos = 0;
    scancode_buffer_writepos = 0;
    CyExitCriticalSection(enableInterrupts);
}

void scan_init(void)
{
    InitSensor();
    EnableSensor();
}

void report_matrix_readouts(void)
{
    for(uint8 i = 0; i<MATRIX_ROWS; i++)
    {
        outbox.response_type = C2RESPONSE_MATRIX_ROW;
        outbox.payload[0] = i;
        outbox.payload[1] = MATRIX_COLS;
        for(uint8_t j=0; j<MATRIX_COLS; j++)
        {
            outbox.payload[2 + j] = matrix[i][j] & 0xff;
        }
        usb_send_c2();
    }
}
