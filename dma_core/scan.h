/*
 *
 * Copyright (C) 2016-2017 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#pragma once
#include "globals.h"

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

void scan_init(uint8_t);
void scan_reset(void);
void scan_start(void);
void scan_nap(void);
void scan_wake(void);
void scan_tick(void);
