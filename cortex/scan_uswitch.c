/*
 *
 * Copyright (C) 2018 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <project.h>

#include "scan.h"

uint16_t prev_row_status = 0;
uint16_t prev_col_status = 0;

CY_ISR(SenseIRQ_ISR) {
  // OK. Logic.
  // Pulse keys hold low for 60 microseconds.
  // This loop supposed to be 200 kHz = 5us/cycle, 12 cycles per pulse.
  // Now. I really hope there will be no smartass pressing 2 keys within 10us.
  // Considering this, I declare single-key transition the only valid one.
  // Single-key transition is a change of exactly one row AND one column.
  // Everything else is considered EMI and ignored.
  uint16_t row_status = R0_Read() + (R1_Read() << 8);
  uint16_t col_status = C0_Read() + (C1_Read() << 8);
  if (row_status == prev_row_status && col_status == prev_col_status) {
    return; // No change this tick, yay!
  }
  uint16_t r_diff = row_status ^ prev_row_status;
  uint16_t c_diff = col_status ^ prev_col_status;
  if ((r_diff & (r_diff - 1)) > 0 || (c_diff & (c_diff - 1)) > 0) {
    // Multiple lines changed, invalid transition.
    return;
  }
  bool row_down = (row_status & r_diff) > 0;
  bool col_down = (col_status & c_diff) > 0;
  if (row_down != col_down) {
    // Both flips must be in the same direction!
    return;
  }
  // OK, single change, same direction. Let's find key position.
  uint8_t row = 0;
  uint8_t col = 0;
  for (uint8_t pos = 0; pos < 16; ++pos) {
    if (TEST_BIT(r_diff, pos)) {
      row = pos;
    }
    if (TEST_BIT(c_diff, pos)) {
      col = pos;
    }
  }
  prev_row_status = row_status;
  prev_col_status = col_status;
  scan_debounce(row_down ? 0 : KEY_UP_MASK, row * MATRIX_COLS + col);
}

void scan_init(uint8_t debouncing_period) {
  status_register &= (1 << C2DEVSTATUS_SETUP_MODE);
  scan_common_init(debouncing_period);
  SenseIRQ_StartEx(SenseIRQ_ISR);
}

void scan_reset() {
  uint8_t enableInterrupts = CyEnterCriticalSection();
  scan_common_reset();
  CyExitCriticalSection(enableInterrupts);
}

void scan_start(void) {
  scan_common_start(1);
}


void scan_nap(void) {
}

void scan_wake(void) {
}

inline void scan_tick(void) {
}
