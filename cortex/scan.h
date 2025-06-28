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
    uint8_t key;
  } __attribute__((packed));
  uint16_t raw;
} scan_event_t;

extern const scan_event_t scan_no_key;

// number of ticks to check for spam after scan starts
#define SANITY_CHECK_DURATION 1000
#define SCANNER_INSANITY_THRESHOLD 5

#define KEY_UP_MASK 0x80

// Flags enabling debug pulses on exp header
#define DEBUG_SHOW_KEYPRESSES 0
#define DEBUG_SHOW_MATRIX_EVENTS 0
#define PROFILE_SCAN_PROCESSING 0

void scan_register_event(uint8_t flags, uint8_t key);
void scan_debounce(uint8_t flags, uint8_t key);
void scan_set_matrix_value(uint8_t keyIndex, int8_t value);
void report_matrix_readouts();

// Returns COMMONSENSE_NOKEY if no events pending
scan_event_t scan_read_event();

void scan_check_matrix();
void scan_sanity_check();

// Basic initialization - constants, essentially
void scan_common_init(uint8_t debounce_period);

// reset things - initialize matrix and buffers.
// If you use ISRs - don't forget to disable interrupts.
void scan_common_reset();

// Primes sensor sanity check.
// If you use ISRs - don't forget to disable interrupts.
void scan_common_start(uint16_t sanity_check_duration);

// Generates "all keys released" signal.
void scan_common_tick();

// To be implemented by a particular scanner

// Power-on
void scan_init(uint8_t debouncing_period);

// Bring scanner into initial state
void scan_reset(void);

// Start operations
void scan_start(void);

// Just before USB suspend
void scan_nap(void);

// Coming out of USB suspend
void scan_wake(void);

// Called every millisecond (or more) as a part of main loop.
void scan_tick(void);

// Report keys that tripped sanity check
void scan_report_insanity();
