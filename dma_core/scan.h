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

// Should be [number of columns per ADC + 1] * 2 (0 is ground, scan sequence is ch1-ch0-ch2-ch0-ch3-ch0..)
#define PTK_CHANNELS 18
// DO NOT FORGET TO UPDATE PTK COMPONENT TO MATCH THIS!

#define ADC_RESOLUTION 12

// IIR 3/4 = 3/2, 7/8 = 7/3, etc
#define IIR_N 7
#define IIR_M_BIT 3

uint8_t matrix[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];

void scan_init(void);
void scan_start(void);
void scan_update(void);

/* [] END OF FILE */
