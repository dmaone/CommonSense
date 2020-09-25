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

#define QUEUE_SIZE 64
// ^ MUST BE POWER OF 2!

#define BUF_NEXT(X) ((X + 1) & (QUEUE_SIZE - 1))

// "Empty" value: "no key" w/cleared flags (so, technically, PRESSED CS_NOKEY).
const scan_event_t scan_no_key = {.key = COMMONSENSE_NOKEY, .flags = 0};

// Ring buffer for detected-but-not-yet-processed keypresses.
static scan_event_t event_queue[QUEUE_SIZE];
static uint8_t q_begin;
static uint8_t q_end;
// volatile q_end was considered (in some scanners ISR enqueues events and main
// loop was seen to lag). Doesn't cause reordering, so we can get away without.

static int8_t matrix[COMMONSENSE_MATRIX_SIZE];
static bool matrix_was_active = false;

static int8_t press_threshold;
static int8_t release_threshold;
static uint8_t scancodes_while_output_disabled = 0;

// After debouncer conversion, matrix_status exists exclusively for spammy keys
// reporting. Can be removed anytime.
// It has value for spammy keys tho - we need to know which keys, not frequency.
#define SCAN_ROW_SHIFT 5
#define SCAN_COL_MASK 0x1f
static uint32_t matrix_status[1 << (8 - SCAN_ROW_SHIFT)];

inline void scan_register_event(uint8_t flags, uint8_t key) {
  if (STATUS_NOT(C2DEVSTATUS_OUTPUT_ENABLED)) {
    if (scancodes_while_output_disabled <= SCANNER_INSANITY_THRESHOLD) {
      // Avoid overflowing counter! Things can get ugly FAST!
      ++scancodes_while_output_disabled;

      // Mark the key as noisy
      SET_BIT(matrix_status[key >> SCAN_ROW_SHIFT], key & SCAN_COL_MASK);
    }
    return;
  }
  if (STATUS_IS(C2DEVSTATUS_SETUP_MODE)) {
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
  uint8_t pos = BUF_NEXT(q_end);
  event_queue[pos].flags = flags;
  event_queue[pos].key = key;
  q_end = pos;
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
  // Skip empty elements that might be there
  while (q_begin != q_end && event_queue[q_begin].raw == scan_no_key.raw) {
    q_begin = BUF_NEXT(q_begin);
  }
  // MOVE the value from ring buffer to output buffer. Erase buffer element.
  // DO NOT optimize into "return if q_begin = q_end" - value is important.
  scan_event_t result = {.raw = event_queue[q_begin].raw};
  event_queue[q_begin].raw = scan_no_key.raw;
#ifdef MATRIX_LEVELS_DEBUG
  xprintf("sc: %d %d @ %d ms, lvl %d/%d", scancode & KEY_UP_MASK,
          scancode, systime,
          level_buffer[q_begin],
          level_buffer_inst[q_begin]);
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
  for(uint8_t i = 0; i < QUEUE_SIZE; ++i) {
    event_queue[i].raw = scan_no_key.raw;
  }
  memset(matrix_status, 0, sizeof(matrix_status));
  q_begin = 0;
  q_end = 0;
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

  for (uint8_t i = 0; i < MATRIX_COLS; ++i) {
    uint8_t sc = start_index + i;
    outbox.payload[2 + i] =
        TEST_BIT(matrix_status[sc >> SCAN_ROW_SHIFT], sc & SCAN_COL_MASK) ? 1
                                                                          : 0;
  }
  usb_send_c2_blocking();
}

#undef BUF_NEXT
#undef QUEUE_SIZE

#undef SCAN_ROW_SHIFT
#undef SCAN_COL_MASK
