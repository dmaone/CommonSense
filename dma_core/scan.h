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

// Should be [number of columns per ADC + 1] * 2 + 1
// TRICKY PART: Count7(which is part of PTK) counts down. 
// So column 0 must be connected to highest input on the MUX
// MUX input 0 must be connected to ground - we use it to discharge ADC sampling cap.
// So, scan sequence code sees is ch0-ch1-ch0-ch2-ch0-ch3-ch0..
#define PTK_CHANNELS 19
// DO NOT FORGET TO UPDATE PTK COMPONENT TO MATCH ^!

// If you ever change the above to the even value - update "+ 1" accordingly! (+0 or +2..)
#define _ADC_COL_OFFSET(i) ((i<<1) + 1)

#define ADC_RESOLUTION 10

#undef COMMONSENSE_100KHZ_MODE
// 0 - none, 1 - 1/2, 2 - 3/4, etc.
// WARNING - uses matrix as accumulator, so order++ = 2*output level!
#define COMMONSENSE_IIR_ORDER 2

uint8_t matrix[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];

void scan_init(void);
void scan_start(void);
void scan_update(void);

/* [] END OF FILE */
