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

// After debouncer conversion, matrix_status exists exclusively for spammy keys
// reporting. Can be removed anytime.
#define SCAN_ROW_SHIFT 5
#define SCAN_COL_MASK 0x1f
uint32_t matrix_status[1 << (8 - SCAN_ROW_SHIFT)];

bool matrix_was_active = 0;

int8_t matrix[COMMONSENSE_MATRIX_SIZE];
int8_t press_threshold;
int8_t release_threshold;
uint8_t scancodes_while_output_disabled = 0;

inline void append_scancode(uint8_t flags, uint8_t code) {
  if (0 == TEST_BIT(status_register, C2DEVSTATUS_OUTPUT_ENABLED)) {
    if (scancodes_while_output_disabled <= SCANNER_INSANITY_THRESHOLD) {
      // Avoid overflowing counter! Things can get ugly FAST!
      ++scancodes_while_output_disabled;

      // Mark the key as noisy
      SET_BIT(matrix_status[code >> SCAN_ROW_SHIFT], code & SCAN_COL_MASK);
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
  scancodes[scancodes_wpos].scancode = code;
}

inline void append_debounced(uint8_t flags, uint8_t keyIndex) {
  int8_t key = matrix[keyIndex];
  /*
   *Stolen from improved xwhatsit firmware. Same algo as before, less bits.
   * Main idea: using sign for key state, abs. value to count the steps.
   * So. Base states: 1 = pressed, -1 = released.
   * Zero is in "released" land, although that only matters on startup.
   * When the key is released and matrix comes pressed: decrement value further.
   * -1 -> -2 -> ... Once -T is reached - flip to 1 and generate keypress.
   * See fundamental https://my.eng.utah.edu/~cs5780/debouncing.pdf
     * listing 2 does ~this using per-key shift registers
     * listing 1 does this with global counter var.
   */
  if (flags & KEY_UP_MASK) { // Matrix position is OFF
    if (key <= 0) { // Still released. Reset the streak counter.
      key = -1;
    } else { // Was pressed. Bump the streak counter.
      if (++key >= release_threshold) { // Long enough. Release the hounds!
        key = -1;
        append_scancode(KEY_UP_MASK, keyIndex);
      }
    }
  } else if (key > 0) { // Pressed, no change. Reset the streak counter.
    key = 1;
  } else { // Known pressed, was released. Bump the streak counter.
    if (--key <= press_threshold) { // Long enough. Press the key.
      key = 1;
      append_scancode(0, keyIndex);
      matrix_was_active = true;
    }
  }
  matrix[keyIndex] = key;
}

inline void scan_set_matrix_value(uint8_t keyIndex, int8_t value) {
  matrix[keyIndex] = value;
}

/*
 * Generates that "matrix is idle" key
 * The idea: 2 low bits of matrix_activity_detector are used:
   * bit 0 for "key was pressed since last check"
   * bit 1 for "key was pressed last time"
 * Longer delays achievable with shifts and bitmask, but not necessary.
 */
inline void scan_check_matrix(void) {
  if (!matrix_was_active) { // idle -> idle
    return;
  }
  matrix_was_active = false; // Covers "key pressed while counting" scenario
  for (uint8_t i = 0; i < COMMONSENSE_MATRIX_SIZE; ++i) {
    if (matrix[i] > 0) {
      matrix_was_active = true;
      break;
    }
  }
  if (!matrix_was_active) { // was active, no longer is.
    append_scancode(KEY_UP_MASK, COMMONSENSE_NOKEY);
  }
}

void scan_sanity_check() {
  if (0 == --sanity_check_timer) {
    // We're out of the woods.
    scan_common_reset();
    pipeline_init();
    SET_BIT(status_register, C2DEVSTATUS_OUTPUT_ENABLED);
  } else if (scancodes_while_output_disabled >= SCANNER_INSANITY_THRESHOLD) {
    // Keyboard is insane. Disable it.
    status_register &= (1 << C2DEVSTATUS_SETUP_MODE); // Keep setup mode.
    SET_BIT(status_register, C2DEVSTATUS_INSANE);
    xprintf("Scan module has gone insane and had to be shot!");
    sanity_check_timer = 0;
  }
}

void scan_common_init(uint8_t steps) {
  /* What I'm doing here:
   * All status values are invalid when scan is reinitialized.
   * BUT
   * I don't want to fuck up the setup mode if we're in setup mode. SO.
   */
  status_register &= (1 << C2DEVSTATUS_SETUP_MODE);

  // Since 1 is used for key state, lowest useful trigger value is 2.
  // +1, not +2, to keep compatibility with previous debouncing algo (which
  // would likely have been crashed firmware for a period of zero).
  // Also remember that press threshold is negative and release is positive.
  release_threshold = steps + 1;
  press_threshold = -release_threshold;
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
    matrix[i] = -1;
  }
}

void scan_common_start(uint16_t sanity_check_duration) {
  // Sanity check - start scanning with no output, if spam is read out, STAHP.
  CLEAR_BIT(status_register, C2DEVSTATUS_OUTPUT_ENABLED);
  SET_BIT(status_register, C2DEVSTATUS_SCAN_ENABLED);
  sanity_check_timer = sanity_check_duration;
  scancodes_while_output_disabled = 0;
}

void scan_common_tick() {
  scan_check_matrix();
}


void report_matrix_readouts(void) {
  uint8_t idx = 0;
  for (uint8 i = 0; i < MATRIX_ROWS; i++) {
    outbox.response_type = C2RESPONSE_MATRIX_ROW;
    outbox.payload[0] = i;
    outbox.payload[1] = MATRIX_COLS;
    for (uint8_t j = 0; j < MATRIX_COLS; j++) {
      outbox.payload[2 + j] = matrix[idx++];
    }
    usb_send_c2_blocking();
  }
}

void scan_report_insanity() {
  static uint8_t cur_row = 0;
  if (cur_row == 0) {
    cur_row = MATRIX_ROWS;
  }
  --cur_row;
  outbox.response_type = C2RESPONSE_MATRIX_ROW;
  outbox.payload[0] = cur_row;
  outbox.payload[1] = MATRIX_COLS;
  uint8_t start_index = MATRIX_COLS * cur_row;

  for (uint8_t i = 0; i < MATRIX_COLS; i++) {
    uint8_t sc = start_index + i;
    if (TEST_BIT(matrix_status[sc >> SCAN_ROW_SHIFT], (sc & SCAN_COL_MASK))) {
      outbox.payload[2 + i] = 1;
    } else {
      outbox.payload[2 + i] = 0;
    }
  }
  usb_send_c2_blocking();
}
