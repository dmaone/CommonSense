/*
 *
 * Copyright (C) 2018 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "scan_common.h"

uint32_t matrix_status[MATRIX_ROWS];
bool matrix_was_active;

inline void append_scancode(uint8_t flags, uint8_t scancode) {
#if DEBUG_SHOW_KEYPRESSES == 1
  if ((scancode & KEY_UP_MASK)) {
    PIN_DEBUG(1, 1)
  } else {
    PIN_DEBUG(1, 2)
  }
#endif
  scancodes_wpos = SCANCODES_NEXT(scancodes_wpos);
  scancodes[scancodes_wpos].flags = flags;
  scancodes[scancodes_wpos].scancode = scancode;
  uint8_t row = scancode >> 5; // uint32 holds 32 values
  uint8_t col = scancode & 0x1f; // 0 to 31
  if (flags & KEY_UP_MASK) {
    CLEAR_BIT(matrix_status[row], col);
  } else {
    SET_BIT(matrix_status[row], col);
    matrix_was_active = true;
  }
}

/*
 * Generates that "matrix is idle" key
 */
inline void scan_check_matrix(void) {
  if (!matrix_was_active) {
    return;
  }
  bool matrix_active_now = false;
  for (uint8_t i=0; i<MATRIX_ROWS; i++) {
    matrix_active_now |= (matrix_status[i] > 0);
  }
  if (!matrix_active_now) {
    append_scancode(KEY_UP_MASK, COMMONSENSE_NOKEY);
    matrix_was_active = false;
  }
}

void init_scancode_buffer() {
  for(uint8_t i = 0; i <= SCANCODE_BUFFER_END; i++) {
    scancode_buffer[i].flags = 0;
    scancode_buffer[i].scancode = COMMONSENSE_NOKEY;
  }
}