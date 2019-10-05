/*
 *
 * Copyright (C) 2018 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "scan.h"

#include <project.h>

#include "PSoC_USB.h"

static uint32_t matrix_status[MATRIX_ROWS];
bool matrix_was_active;

#define MAX_MATRIX_VALUE 0xffff
uint16_t matrix[COMMONSENSE_MATRIX_SIZE];
uint16_t debouncing_mask;
uint16_t debouncing_posedge;
uint16_t debouncing_negedge;
uint8_t scancodes_while_output_disabled = 0;

inline void append_scancode(uint8_t flags, uint8_t scancode) {
  if (0 == TEST_BIT(status_register, C2DEVSTATUS_OUTPUT_ENABLED)) {
    if (scancodes_while_output_disabled <= SCANNER_INSANITY_THRESHOLD) {
      // Avoid overflowing counter! Things can get ugly FAST!
      ++scancodes_while_output_disabled;
    }
    return;
  }

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

inline void append_debounced(uint8_t flags, uint8_t keyIndex) {
  if (flags & KEY_UP_MASK) {
    // Release
    matrix[keyIndex] = ((matrix[keyIndex] << 1) | debouncing_mask);
  } else {
    // Press
    matrix[keyIndex] = ((matrix[keyIndex] << 1) | debouncing_mask) + 1;
  }
  // Do not try to optimize for checking keyDown once. Short circuiting will
  // take care of that and will be faster - 1 cmp in most checks, no bit ops.
  if (matrix[keyIndex] == debouncing_posedge &&
      !scan_is_key_down(keyIndex)) {
    append_scancode(0, keyIndex);
  } else if (matrix[keyIndex] == debouncing_negedge &&
             scan_is_key_down(keyIndex)) {
    append_scancode(KEY_UP_MASK, keyIndex);
  }
}

inline void scan_set_matrix_value(uint8_t keyIndex, uint16_t value) {
  matrix[keyIndex] = value;
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

inline bool scan_is_key_down(uint8_t keyIndex) {
  return matrix_status[keyIndex >> 5] & (1 << (keyIndex & 0x1f));
}

void scan_sanity_check() {
  if (scancodes_while_output_disabled >= SCANNER_INSANITY_THRESHOLD) {
    // Keyboard is insane. Disable it.
    status_register &= (1 << C2DEVSTATUS_SETUP_MODE); // Keep setup mode.
    SET_BIT(status_register, C2DEVSTATUS_INSANE);
    xprintf("Scan module has gone insane and had to be shot!");
    sanity_check_timer = 0;
  } else if (0 == --sanity_check_timer) {
    SET_BIT(status_register, C2DEVSTATUS_OUTPUT_ENABLED);
  }
}

void scan_common_init(uint8_t debounce_period) {
  /* What I'm doing here:
   * All status values are invalid when scan is reinitialized.
   * BUT
   * I don't want to fuck up the setup mode if we're in setup mode. SO.
   */
  status_register &= (1 << C2DEVSTATUS_SETUP_MODE);

  // Init debouncing parameters.
  // Example: 8 bits total, 4 debouncing steps.
  // Negative/falling edge: xxxx 1000 - pressed, followed by 3 released.
  // Positive/raising edge: xxxx 0111 - released, followed by 3 pressed.
  debouncing_mask = MAX_MATRIX_VALUE << debounce_period;
  debouncing_negedge = MAX_MATRIX_VALUE << (debounce_period - 1);
  debouncing_posedge = ~debouncing_negedge | debouncing_mask;
}

void scan_common_reset() {
  for(uint8_t i = 0; i <= SCANCODES_END; i++) {
    scancodes[i].flags = 0;
    scancodes[i].scancode = COMMONSENSE_NOKEY;
  }
  memset(matrix_status, 0, sizeof(matrix_status));
  scancodes_rpos = 0;
  scancodes_wpos = 0;
  for (uint8_t i = 0; i < COMMONSENSE_MATRIX_SIZE; i++) {
#if NORMALLY_LOW == 1
    matrix[i] = 0;
#else
    matrix[i] = MAX_MATRIX_VALUE;
#endif
  }
  memset(matrix_status, 0, sizeof(matrix_status));
}

void scan_common_start(uint16_t sanity_check_duration) {
  // Sanity check - start scanning with no output, if spam is read out, STAHP.
  CLEAR_BIT(status_register, C2DEVSTATUS_OUTPUT_ENABLED);
  SET_BIT(status_register, C2DEVSTATUS_SCAN_ENABLED);
  sanity_check_timer = sanity_check_duration;
  scancodes_while_output_disabled = 0;
}

void report_matrix_readouts(void) {
  uint8_t idx = 0;
  for (uint8 i = 0; i < MATRIX_ROWS; i++) {
    outbox.response_type = C2RESPONSE_MATRIX_ROW;
    outbox.payload[0] = i;
    outbox.payload[1] = MATRIX_COLS;
    for (uint8_t j = 0; j < MATRIX_COLS; j++) {
      outbox.payload[2 + j] = matrix[idx++] & 0xff;
    }
    usb_send_c2_blocking();
  }
}
