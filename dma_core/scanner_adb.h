/*
 *
 * Copyright (C) 2016-2018 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#pragma once
#include "globals.h"

#define ADDR_KEYBOARD 0x20

typedef union {
  struct {
    uint8_t key0;
    uint8_t key1;
  };
  uint16_t raw;
} adb_pdu_t;


void scan_init(uint8_t);
void scan_start(void);
void scan_tick(void);
void scan_reset(void);
void scan_nap(void);
void scan_wake(void);
void scan_sanity_check(void);
void report_matrix_readouts(void);
void sync_leds(void);
