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

// This is to ease calculations, there are things hardcoded in buffer
// management!! For 1 and 2 ADCs, that is.
#define NUM_ADCs 1

#define ADC_CHANNELS (MATRIX_COLS / NUM_ADCs)

// Should be [number of columns per ADC + 1] * 2 + 1 - so 19 for MF, 27 for BS
// TRICKY PART: Count7(which is part of PTK) counts down.
// So column 0 must be connected to highest input on the MUX
// MUX input 0 must be connected to ground - we use it to discharge ADC sampling
// cap. So, scan sequence code sees is ch0-ch1-ch0-ch2-ch0-ch3-ch0..
#define PTK_CHANNELS (2 * ADC_CHANNELS + 3)

// For even values of the above - both offsets need to be updated!
#define ADC_BUF_INITIAL_OFFSET 1
#define ADC_BUF_INTER_ROW_GAP 3

// Below is per ADC.
#define ADC_BUFFER_BYTESIZE (PTK_CHANNELS * 2)

// Don't forget to set PTK to 5 channels for 100kHz mode!
// 3 channels is too low - pulse reset logic activates at ch2 selection
// Not good, 2 being the first channel in 3-channel config!
// PTK calibration: 5 = 114kHz, 7 - 92kHz, 15 - 52kHz
#undef COMMONSENSE_100KHZ_MODE

#if NORMALLY_LOW == 1
#define ADC_CMP_OP >
#else
#define ADC_CMP_OP <
#endif

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
  PTK_Start();
  PTK_WritePeriod(PTK_CHANNELS - 1);
  PTK_WriteCounter(PTK_CHANNELS - 1); // Just in case!

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
  ControlReg0_Write(1); // Enable sensor
  /*
   * The above needs to come from software.
   * It's not like hardware CANNOT do it - but it will require two-stage OR,
   * followed by NOT (because sensor is enabled by disabling output on columns).
   * This is _sometimes_ compiled into a NOT with a 60us delay (which, in turn,
   * manifests as a missing "sensor enable" signal for the highest row).
   * It's easier to just enable sensor from CPU, avoiding whole issue.
   * Yes, this starts ChargeDelay - just drive the damn DriveReg right away.
   */
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
        || STATUS_NOT(C2DEVSTATUS_SCAN_ENABLED)) {
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
    if (STATUS_IS(C2DEVSTATUS_TELEMETRY_MODE)) {
      // When monitoring matrix we're interested in raw feed.
      // We also want to see all the keys, even ignored ones.
      scan_set_matrix_value(--keyIndex, Results[adc_buffer_pos]);
      continue;
    }
    const uint8_t threshold = config.thresholds[--keyIndex];
    if (threshold == SKIP_SCAN) {
      continue; // As if nothing happened!
    }
    if (Results[adc_buffer_pos] ADC_CMP_OP threshold) {
      scan_debounce(0, keyIndex);
#if DEBUG_SHOW_MATRIX_EVENTS == 1
      PIN_DEBUG(4, 1);
#endif
    } else {
      scan_debounce(KEY_UP_MASK, keyIndex);
    }
  }
#if PROFILE_SCAN_PROCESSING == 1
  CyPins_ClearPin(ExpHdr_1);
#endif
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

void scan_tick() {
  scan_common_tick();
};
