/*
 *
 * Copyright (C) 2018 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#pragma once
#include "globals.h"

typedef union {
  struct {
    uint8_t flags;
    uint8_t scancode;
  } __attribute__((packed));
  uint8_t raw[2];
} scancode_t;

// IMPORTANT - MUST NOT BE A REAL KEY! Easy for beamspring, less so for F122
// with it's 8x16 matrix.
#define COMMONSENSE_NOKEY 0xff

#define KEY_UP_MASK 0x80
#define SCANCODE_MASK 0x7f

// Flags enabling debug pulses on exp header
#define DEBUG_SHOW_KEYPRESSES 0
#define DEBUG_SHOW_MATRIX_EVENTS 0
#define PROFILE_SCAN_PROCESSING 0

#define SCANCODES_END 31
#define SCANCODES_NEXT(X) ((X + 1) & SCANCODES_END)
// ^^^ THIS MUST EQUAL 2^n-1!!! Used as bitmask.

scancode_t scancodes[SCANCODES_END + 1];
uint8_t scancodes_wpos;
uint8_t scancodes_rpos;

void append_scancode(uint8_t flags, uint8_t scancode);
void scan_check_matrix();
uint32_t scan_is_key_pressed(uint8_t keyIndex);
void init_scan_common();