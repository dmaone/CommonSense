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

typedef struct {
    uint32_t sysTick;
    uint8_t flags;
    uint8_t scancode;
} queuedScancode;

queuedScancode USBQueue[128];
uint8_t USBQueue_writepos;

void do_pipeline(void);