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

// 0 - none, 1 - 1/2, 2 - 3/4, etc.
// WARNING - uses matrix as accumulator, so order++ = 2*output level!
#define COMMONSENSE_IIR_ORDER 2

// SYNC WITH PTK CHANNELS!!! MANUALLY BECAUSE GUI ACTIONS REQUIRED!!!
#define ADC_CHANNELS 12

// Should be [number of columns per ADC + 1] * 2 + 1 - so 19 for MF, 27 for BS
// TRICKY PART: Count7(which is part of PTK) counts down. 
// So column 0 must be connected to highest input on the MUX
// MUX input 0 must be connected to ground - we use it to discharge ADC sampling cap.
// So, scan sequence code sees is ch0-ch1-ch0-ch2-ch0-ch3-ch0..
#define PTK_CHANNELS 27
// DO NOT FORGET TO UPDATE PTK COMPONENT TO MATCH ^!

// If you ever change the above to the even value - update "+ 1" accordingly! (+0 or +2..)
#define _ADC_COL_OFFSET(i) ((i<<1) + 1)

#define ADC_RESOLUTION 10



// Don't forget to set PTK to 5 channels for 100kHz mode! 
// 3 channels is too low - pulse reset logic activates at ch2 selection
// Not good, 2 being the first channel in 3-channel config!
// PTK calibration: 5 = 114kHz, 7 - 92kHz, 15 - 52kHz
#undef COMMONSENSE_100KHZ_MODE

uint16_t matrix[MATRIX_ROWS][MATRIX_COLS+1]; // Need to leave space for even number of columns.

uint8_t scancode_buffer[32];
uint8_t scancode_buffer_writepos;
uint8_t scancode_buffer_readpos;

void scan_init(void);
void scan_start(void);

//TODO move to scancode processor module.
typedef struct {
    uint32_t sysTick;
    uint8_t flags;
    uint8_t scancode;
} queuedScancode;

queuedScancode USBQueue[128];
uint8_t USBQueue_writepos;

void process_scancode_buffer(void);

/* [] END OF FILE */
