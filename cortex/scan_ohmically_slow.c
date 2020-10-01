/*
 *
 * Copyright (C) 2020 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <project.h>

#include "scan.h"

static uint8_t current_row;

void scan_init(uint8_t debouncing_period) {
  status_register &= (1 << C2DEVSTATUS_SETUP_MODE);
  scan_common_init(debouncing_period);
}

void scan_reset() {
  uint8_t enableInterrupts = CyEnterCriticalSection();
  scan_common_reset();
  CyExitCriticalSection(enableInterrupts);
}

void scan_start(void) {
  scan_common_start(SANITY_CHECK_DURATION);
  current_row = 0;
}


void scan_nap(void) {
}

void scan_wake(void) {
}

inline void scan_tick(void) {
  uint8_t readout = C0_Read(); // Reading row N (N < MATRIX_ROWS!)
  uint8_t base_key_index = current_row * MATRIX_COLS;
  if (current_row == 0) {
    current_row = MATRIX_ROWS;
  }
  R0_Write(1 << (--current_row)); // ..select row N-1
  // We trigger row here to give switches time to stabilize before next read.
  // Makes the scanner slow (1000 rows per second) - but nobody needs faster.

  for (uint8_t i = 0; i < MATRIX_COLS; ++i) {
    scan_debounce((readout & (1 << i)) ? 0 : KEY_UP_MASK, base_key_index + i);
  }
  scan_common_tick();
}
