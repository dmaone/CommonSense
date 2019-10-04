/*
 *
 * Copyright (C) 2016-2017 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "scan.h"

#include <project.h>

#include "scan_common.h"

CY_ISR_PROTO(EoC_ISR);
CY_ISR_PROTO(Result_ISR);

uint8_t Buf0TD = CY_DMA_INVALID_TD;
uint8_t Buf1TD = CY_DMA_INVALID_TD;
uint8_t FinalBufTD[2];
uint16_t BufMem[PTK_CHANNELS * NUM_ADCs];

// We're only using low 8 bit of ADC output, but ADC gets us 16 and then 
uint8_t Results[ADC_CHANNELS * 4 * NUM_ADCs];

uint8_t reading_row, driving_row;
bool scan_in_progress;

void BufferSetup(uint8 chan, uint8 *td, uint8 channel_config,
                        uint32 src_addr, uint32 dst_addr) {
  (void)CyDmaClearPendingDrq(chan);
  if (*td == CY_DMA_INVALID_TD)
    *td = CyDmaTdAllocate();
  // transferCount is actually bytes, not transactions.
  (void)CyDmaTdSetConfiguration(*td, (uint16)ADC_BUFFER_BYTESIZE, *td,
                                (channel_config | (uint8)TD_INC_DST_ADR));
  (void)CyDmaTdSetAddress(*td, LO16(src_addr), LO16(dst_addr));
  (void)CyDmaChSetInitialTd(chan, *td);
  (void)CyDmaChEnable(chan, 1);
}

void ResultBufferSetup(void) {
  CyDmaClearPendingDrq(FinalBuf_DmaHandle);
  FinalBufTD[0] = CyDmaTdAllocate();
#if NUM_ADCs == 1
  CyDmaTdSetConfiguration(
      FinalBufTD[0], (uint16)(sizeof Results), FinalBufTD[0],
      CY_DMA_TD_INC_SRC_ADR | CY_DMA_TD_INC_DST_ADR | FinalBuf__TD_TERMOUT_EN);
  CyDmaTdSetAddress(FinalBufTD[0],
                    LO16((uint32)&BufMem[ADC_BUF_INITIAL_OFFSET]),
                    LO16((uint32)&Results));
#elif NUM_ADCs == 2
  FinalBufTD[1] = CyDmaTdAllocate();
  // transferCount is actually bytes, not transactions.
  CyDmaTdSetConfiguration(
      FinalBufTD[0], (uint16)(sizeof Results / NUM_ADCs), FinalBufTD[1],
      CY_DMA_TD_INC_SRC_ADR | CY_DMA_TD_INC_DST_ADR | CY_DMA_TD_AUTO_EXEC_NEXT);
  CyDmaTdSetAddress(FinalBufTD[0],
                    LO16((uint32)&BufMem[ADC_BUF_INITIAL_OFFSET]),
                    LO16((uint32)&Results));
  CyDmaTdSetConfiguration(
      FinalBufTD[1], (uint16)(sizeof Results / NUM_ADCs), FinalBufTD[0],
      CY_DMA_TD_INC_SRC_ADR | CY_DMA_TD_INC_DST_ADR | FinalBuf__TD_TERMOUT_EN);
  CyDmaTdSetAddress(
      FinalBufTD[1],
      LO16((uint32)&BufMem[PTK_CHANNELS + ADC_BUF_INITIAL_OFFSET]),
      LO16((uint32)&Results[ADC_CHANNELS * 2]));
#else
#error only 1 and 2 ADCs are supported
#endif
  CyDmaChSetInitialTd(FinalBuf_DmaHandle, FinalBufTD[0]);
  CyDmaChEnable(FinalBuf_DmaHandle, 1);
}

void sensor_init() {
  // Init DMA, each burst requires a request
  Buf0_DmaInitialize(sizeof BufMem[0], 1, (uint16)(HI16(CYDEV_PERIPH_BASE)),
                     (uint16)(HI16(CYDEV_SRAM_BASE)));
#if NUM_ADCs > 1
  Buf1_DmaInitialize(sizeof BufMem[0], 1, (uint16)(HI16(CYDEV_PERIPH_BASE)),
                     (uint16)(HI16(CYDEV_SRAM_BASE)));
#endif
  // 1 request per ADC, get the whole ADC buffer (skip grounded channels which
  // are at the end).
  FinalBuf_DmaInitialize(sizeof Results / NUM_ADCs, NUM_ADCs,
                         (uint16)(HI16(CYDEV_SRAM_BASE)),
                         (uint16)(HI16(CYDEV_SRAM_BASE)));
  uint8 enableInterrupts = CyEnterCriticalSection();
  (*(reg8 *)PTK_ChannelCounter__PERIOD_REG) =
      (PTK_CHANNELS -
       1); // Load number of channels. See Count7/WritePeriod for details.
  (*(reg8 *)PTK_ChannelCounter__CONTROL_AUX_CTL_REG) |=
      (uint8)0x20u; // Init count7
  CyExitCriticalSection(enableInterrupts);
  ADC0_Start();
  ADC0_SetResolution(config.adcBits);
#if NUM_ADCs > 1
  ADC1_Start();
  ADC1_SetResolution(config.adcBits);
#endif
  ChargeDelay_Start();
  ChargeDelay_WritePeriod(config.chargeDelay);
  DischargeDelay_Start();
  DischargeDelay_WritePeriod(config.dischargeDelay);

  BufferSetup(Buf0_DmaHandle, &Buf0TD, Buf0__TD_TERMOUT_EN,
              (uint32)ADC0_ADC_SAR__WRK0, (uint32)BufMem);
#if NUM_ADCs > 1
  BufferSetup(Buf1_DmaHandle, &Buf1TD, Buf1__TD_TERMOUT_EN,
              (uint32)ADC1_ADC_SAR__WRK0, ADC_BUFFER_BYTESIZE + (uint32)BufMem);
#endif
  ResultBufferSetup();
  (*(reg8 *)PTK_CtrlReg__CONTROL_REG) =
      (uint8)0b11u; // enable counter's clock, generate counter load pulse
  // It is important that ResultIRQ priority is less (=is higher)
  // so we don't start fetching next buffer before we are done with the current
  // one.
  ResultIRQ_StartEx(Result_ISR);
  EoCIRQ_StartEx(EoC_ISR);
}

void sensor_nap(void) {
  ChargeDelay_Sleep();
  DischargeDelay_Sleep();
  ADC0_Sleep();
}

void sensor_wake(void) {
  ADC0_Wakeup();
  DischargeDelay_Wakeup();
  ChargeDelay_Wakeup();
}

static inline void Drive(uint8 drv) {
  /*
   * CyPins_SetPinDriveMode is HELLISHLY expensive (about 1us/call)
   * So changing drive mode in realtime to enhance crosstalk immunity is _not_ a
   * good idea. reading the row in 4us is pointless if you spend 20us setting
   * drive modes.
   */
  // SetPin should not be used because it doesn't trigger start circuitry
  DriveReg0_Write(1 << drv);
}

CY_ISR(EoC_ISR) {
#ifdef DEBUG_INTERRUPTS
  PIN_DEBUG(1, 1)
#endif
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
  if (0 == driving_row) {
    // End of the scan pass. Loop if full throttle, otherwise stop.
    if (power_state != DEVSTATE_FULL_THROTTLE
        || 0 == TEST_BIT(status_register, C2DEVSTATUS_SCAN_ENABLED)) {
      scan_in_progress = false;
      goto EoC_final; // Important - otherwise interrupts are left disabled!
    }
    driving_row = MATRIX_ROWS;
  }
  driving_row--;
  // Drive row.
  // DMA channel reading out results has priority, so this should not overwrite
  // the results buffer.
  Drive(driving_row);

EoC_final:
  CyExitCriticalSection(enableInterrupts);
}

CY_ISR(Result_ISR) {
#ifdef DEBUG_INTERRUPTS
  PIN_DEBUG(1, 2)
#endif
#ifdef COMMONSENSE_100KHZ_MODE
  return;
// The rest of the code is dead in 100kHz mode.
#endif
  int8_t adc_buffer_pos = -4;
  // keyIndex - same speed as static global on -O3, faster in -Os
  // having uint16_t* for cell is slower than directly using matrix[keyIndex].
  uint8_t keyIndex = (reading_row + 1) * MATRIX_COLS;
  // caching row status is faster than direct access, but common functions..
#if PROFILE_SCAN_PROCESSING == 1
  CyPins_SetPin(ExpHdr_1);
#endif
  for (int8_t curCol = ADC_CHANNELS * NUM_ADCs - 1; curCol >= 0; curCol--) {
    adc_buffer_pos += 4;
    // TEST_BIT is faster than bool inited outside of the loop.
    if (TEST_BIT(status_register, C2DEVSTATUS_MATRIX_MONITOR)) {
      // When monitoring matrix we're interested in raw feed.
      scan_set_matrix_value(--keyIndex, Results[adc_buffer_pos]);
      continue;
#if NORMALLY_LOW == 1
    } else if (Results[adc_buffer_pos] > config.thresholds[--keyIndex]) {
#else
    } else if (Results[adc_buffer_pos] < config.thresholds[--keyIndex]) {
#endif
      append_debounced(0, keyIndex);
#if DEBUG_SHOW_MATRIX_EVENTS == 1
      PIN_DEBUG(4, 1);
#endif
    } else {
      append_debounced(KEY_UP_MASK, keyIndex);
    }
  }
#if PROFILE_SCAN_PROCESSING == 1
  CyPins_ClearPin(ExpHdr_1);
#endif
  if (reading_row == 0) {
    // End of matrix reading cycle.
    scan_check_matrix();
  }
}

void scan_init(uint8_t debouncing_period) {
  status_register &= (1 << C2DEVSTATUS_SETUP_MODE);
  while (scan_in_progress) {}; // Make sure scan is stopped.
  scan_common_init(debouncing_period);
  sensor_init();
}

void scan_reset() {
  uint8_t enableInterrupts = CyEnterCriticalSection();
  scan_common_reset();
  CyExitCriticalSection(enableInterrupts);
}

void scan_start(void) {
  if (scan_in_progress) {
    return;
  }
  scan_common_start(SANITY_CHECK_DURATION);
  // Set things into "end of the cycle" position, then let magic happen.
  // Will cause a random first readout - but debouncing will take care of that.
  driving_row = MATRIX_ROWS - 1;
  Drive(driving_row);
  scan_in_progress = true;
}

void scan_nap(void) {
  while (scan_in_progress) {} // Unsafe to proceed with interrupts flying around
  sensor_nap();
}

void scan_wake(void) {
  sensor_wake();
}

inline void scan_tick() {};
