/*
 *
 * Copyright (C) 2023 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "hid.h"
#include "io.h"

void hid_init(void) {
  memset(keyboard_state.raw, 0, sizeof keyboard_state.raw);
  keys_pressed = 0;
  memset(consumer_report, 0, sizeof consumer_report);
}

#define IO_FUNCNAME(R) io_##R
#define RESET_SINGLE(R)                                                        \
  memset(R##_state, 0, sizeof R##_state);                                      \
  if (memcmp(R##_state, R##_report, sizeof R##_report) != 0) {                 \
    memset(R##_report, 0, sizeof R##_report);                                  \
    IO_FUNCNAME(R)((void*)R##_report);                                                \
  }

/*
 * updates host only if there were stuck keys, to avoid N reports on every "last
 * key depressed".
 */
void hid_reset_reports(void) {
  memset(keyboard_state.raw, 0, sizeof(keyboard_state.raw));
  if (memcmp(keyboard_state.raw, keyboard_report, sizeof(keyboard_report))) {
    memset(keyboard_report, 0, sizeof(keyboard_report));
    io_keyboard((void*)keyboard_report);
  }
  keys_pressed = 0;
  RESET_SINGLE(consumer)
  RESET_SINGLE(system)
  // xprintf("reports reset");
}

// Very similar to consumer_press, but keycode there is uint16_t :(
inline void keyboard_press(uint8_t keycode) {
  if ((keycode & 0xf8) == 0xe0) {
    keyboard_state.mods |= (1 << (keycode & 0x07));
    return;
  }
  for (uint8_t cur_pos = 0; cur_pos < sizeof keyboard_state.keys; cur_pos++) {
    if (keyboard_state.keys[cur_pos] == keycode) {
      xprintf("Existing %d pos %d", keycode, cur_pos);
      return;
    } else if (keyboard_state.keys[cur_pos] == 0) {
      keyboard_state.keys[cur_pos] = keycode;
      keys_pressed = cur_pos + 1;
      // xprintf("Pressed %d pos %d, usage %d", keycode, cur_pos,
      // keys_pressed);
      return;
    }
  }
}

// Very similar to consumer_release, but keycode there is uint16_t :(
inline void keyboard_release(uint8_t keycode) {
  if ((keycode & 0xf8) == 0xe0) {
    keyboard_state.mods &= ~(1 << (keycode & 0x07));
    return;
  }
  for (uint8_t cur_pos = 0; cur_pos < sizeof keyboard_state.keys; cur_pos++) {
    if (keyboard_state.keys[cur_pos] == keycode) {
      // Key was, in fact, pressed.
      keyboard_state.keys[cur_pos] = keyboard_state.keys[--keys_pressed];
      keyboard_state.keys[keys_pressed] = 0;
      // xprintf("Released %d, usage %d", keycode, keys_pressed);
      return;
    }
  }
}

void hid_update_keyboard(hid_event* event) {
  // xprintf("Updating report for %d", key->keycode);
  if ((event->flags & HID_RELEASED_MASK) == 0) {
    keyboard_press(event->code);
  } else {
    keyboard_release(event->code);
  }
  memcpy(keyboard_report, keyboard_state.raw, sizeof(keyboard_report));
  if (keys_pressed > KBD_KRO_LIMIT) {
    // on rollover error ALL keys must report ERO (Error: Rollover).
    memset(keyboard_report + 2, 1, KBD_KRO_LIMIT); // ERO is scancode 1.
    xprintf("Keyboard rollover error");
  }
  io_keyboard(keyboard_report);
}

const uint16_t consumer_mapping[16] = {
    0xcd, // Play/pause
    0xe2, // Mute
    0xe9, // Vol++
    0xea, // Vol--
    0xb8, // Eject
    0x00, 0x00, 0x00,
    // 8
    0xb0, // Play
    0xb1, // Pause
    0xb2, // Record
    0xb3, // Fwd
    0xb4, // Rev
    0xb5, // NTrk
    0xb6, // PTrk
    0xb7  // Stop
    // 16
};

// Very similar to keyboard_press, but keycode there is uint8_t :(
static inline void consumer_press(uint16_t code) {
  for (uint8_t cur_pos = 0; cur_pos < CONSUMER_KRO_LIMIT; cur_pos++) {
    if (consumer_state[cur_pos] == code) {
      xprintf("Existing %d pos %d", code, cur_pos);
      break;
    } else if (consumer_state[cur_pos] == 0) {
      consumer_state[cur_pos] = code;
      // xprintf("Pressed %d pos %d", code, cur_pos);
      break;
    }
  }
  // xprintf("C_Pressing %d", code);
}

static inline void consumer_release(uint16_t keycode) {
  uint8_t cur_pos;
  bool move = false;
  for (cur_pos = 0; cur_pos < CONSUMER_KRO_LIMIT; cur_pos++) {
    if (move) {
      consumer_state[cur_pos - 1] = consumer_report[cur_pos];
    } else if (consumer_state[cur_pos] == keycode) {
      move = true;
    }
  }
  if (move) {
    // Key was, in fact, pressed.
    consumer_state[CONSUMER_KRO_LIMIT - 1] = 0;
    // xprintf("C_Released %d", keycode);
  }
}

void hid_update_consumer(hid_event* event) {
  // xprintf("Updating report for %d", key->keycode);
  uint16_t code = consumer_mapping[event->code - 0xe8];
  if ((event->flags & HID_RELEASED_MASK) == 0) {
    consumer_press(code);
  } else {
    consumer_release(code);
  }
  memcpy(consumer_report, consumer_state, sizeof(consumer_report));
  io_consumer(consumer_report);
}

void hid_update_system(hid_event* event) {
  uint8_t keyIndex = event->code - 0xa5;
  if ((event->flags & HID_RELEASED_MASK) == 0) {
    system_report[0] |= (1 << keyIndex);
  } else {
    system_report[0] &= ~(1 << keyIndex);
  }
  memcpy(system_report, system_state, sizeof(system_report));
  // xprintf("System: %d", SYSTEM_OUTBOX[0]);  
  io_system(system_report);
}
