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

//IMPORTANT - MUST NOT BE A REAL KEY! Easy for beamspring, less so for F122 with it's 8x16 matrix.
#define COMMONSENSE_NOKEY 0x7f

#define KEY_UP_MASK 0x80
#define SCANCODE_MASK 0x7f

// 0 - none, 1 - 1/2, 2 - 3/4, etc.
// WARNING - uses matrix as accumulator, so order++ = 2*output level!
#define COMMONSENSE_IIR_ORDER 2

#define ADC_RESOLUTION 10

// This is to ease calculations, there are things hardcoded in buffer management!!
#define NUM_ADCs 2

#define ADC_CHANNELS (MATRIX_COLS / NUM_ADCs)

// Should be [number of columns per ADC + 1] * 2 + 1 - so 19 for MF, 27 for BS
// TRICKY PART: Count7(which is part of PTK) counts down. 
// So column 0 must be connected to highest input on the MUX
// MUX input 0 must be connected to ground - we use it to discharge ADC sampling cap.
// So, scan sequence code sees is ch0-ch1-ch0-ch2-ch0-ch3-ch0..
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

#define SCANCODE_BUFFER_END 31
#define SCANCODE_BUFFER_NEXT(X) ((X + 1) & SCANCODE_BUFFER_END)
// ^^^ THIS MUST EQUAL 2^n-1!!! Used as bitmask.

#undef MATRIX_LEVELS_DEBUG
uint8_t scancode_buffer[SCANCODE_BUFFER_END + 1];
#ifdef MATRIX_LEVELS_DEBUG
uint8_t level_buffer[SCANCODE_BUFFER_END + 1];
uint8_t level_buffer_inst[SCANCODE_BUFFER_END + 1];
#endif
uint8_t scancode_buffer_writepos;
uint8_t scancode_buffer_readpos;

void scan_init(void);
void scan_start(void);
void scan_reset(void);
void report_matrix_readouts(void);
