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

#define SCANNER_RING_BUFFER_SIZE 64
// ^ MUST BE POWER OF 2!

#define ADVANCE(X) X = (X + 1) & (SCANNER_RING_BUFFER_SIZE - 1)

// "Empty" value: "no key" w/cleared flags (so, technically, PRESSED CS_NOKEY).
const scan_event_t scan_no_key = {.key = COMMONSENSE_NOKEY, .flags = 0};

// REMINDER: can't make anything here static, because inline functions.
// Take care not to use any, technically, globals, outside scan_common.

// Ring buffer for detected-but-not-yet-processed keypresses.
scan_event_t pending_events[SCANNER_RING_BUFFER_SIZE];
uint8_t write_cursor;
uint8_t read_cursor;

// After debouncer conversion, matrix_status exists exclusively for spammy keys
// reporting. Can be removed anytime.
// It has value for spammy keys tho - we need to know which keys, not frequency.
#define SCAN_ROW_SHIFT 5
#define SCAN_COL_MASK 0x1f
uint32_t matrix_status[1 << (8 - SCAN_ROW_SHIFT)];

bool matrix_was_active = false;

int8_t matrix[COMMONSENSE_MATRIX_SIZE];
int8_t press_threshold;
int8_t release_threshold;
uint8_t scancodes_while_output_disabled = 0;

inline void scan_register_event(uint8_t flags, uint8_t key) {
  if (0 == TEST_BIT(status_register, C2DEVSTATUS_OUTPUT_ENABLED)) {
    if (scancodes_while_output_disabled <= SCANNER_INSANITY_THRESHOLD) {
      // Avoid overflowing counter! Things can get ugly FAST!
      ++scancodes_while_output_disabled;

      // Mark the key as noisy
      SET_BIT(matrix_status[key >> SCAN_ROW_SHIFT], key & SCAN_COL_MASK);
    }
    return;
  }
  if (TEST_BIT(status_register, C2DEVSTATUS_SETUP_MODE)) {
    // In setup mode all matrix events are sent via control channel, not HID.
    // This is done to prevent key spam in case of unfortunate threshold change.
    // Sanity checker should protect from this, but just in case..
    outbox.response_type = C2RESPONSE_SCANCODE;
    outbox.payload[0] = flags;
    outbox.payload[1] = key;
    usb_send_c2();
    return;
  }

#if DEBUG_SHOW_KEYPRESSES == 1
  if ((scancode & KEY_UP_MASK)) {
    PIN_DEBUG(1, 1)
  } else {
    PIN_DEBUG(1, 2)
  }
#endif
  ADVANCE(write_cursor);
  pending_events[write_cursor].flags = flags;
  pending_events[write_cursor].key = key;
}

inline void scan_debounce(uint8_t flags, uint8_t key) {
  int8_t d_buf = matrix[key];
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
    if (d_buf <= 0) { // Still released. Reset the streak counter.
      d_buf = -1;
    } else { // Was pressed. Bump the streak counter.
      if (++d_buf >= release_threshold) { // Long enough. Release the hounds!
        d_buf = -1;
        scan_register_event(KEY_UP_MASK, key);
      }
    }
  } else if (d_buf > 0) { // Pressed, no change. Reset the streak counter.
    d_buf = 1;
  } else { // Known pressed, was released. Bump the streak counter.
    if (--d_buf <= press_threshold) { // Long enough. Press the key.
      d_buf = 1;
      scan_register_event(0, key);
      matrix_was_active = true;
    }
  }
  matrix[key] = d_buf;
}

// For telemetry reporting - cells are used as raw readout value storage.
inline void scan_set_matrix_value(uint8_t key, int8_t value) {
  matrix[key] = value;
}

inline scan_event_t scan_read_event(void) {
  if (read_cursor == write_cursor) {
    // Nothing to read.
    return scan_no_key;
  }
  // Skip empty elements that might be there
  while (pending_events[read_cursor].raw == scan_no_key.raw) {
    ADVANCE(read_cursor);
  }
  // MOVE the value from ring buffer to output buffer. Erase buffer element.
  scan_event_t result = pending_events[read_cursor];
  pending_events[read_cursor].raw = scan_no_key.raw;
#ifdef MATRIX_LEVELS_DEBUG
  xprintf("sc: %d %d @ %d ms, lvl %d/%d", scancode & KEY_UP_MASK,
          scancode, systime,
          level_buffer[read_cursor],
          level_buffer_inst[read_cursor]);
#endif
  return result;
}


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
    scan_register_event(KEY_UP_MASK, COMMONSENSE_NOKEY);
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
  for(uint8_t i = 0; i < SCANNER_RING_BUFFER_SIZE; i++) {
    pending_events[i].raw = scan_no_key.raw;
  }
  memset(matrix_status, 0, sizeof(matrix_status));
  read_cursor = 0;
  write_cursor = 0;
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
    outbox.response_type = C2RESPONSE_TELEMETRY_ROW;
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
  outbox.response_type = C2RESPONSE_TELEMETRY_ROW;
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

#undef ADVANCE
