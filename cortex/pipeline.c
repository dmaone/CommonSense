/*
 *
 * Copyright (C) 2016-2017 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "pipeline.h"

#include <project.h>

#include "gpio.h"
#include "hid.h"
#include "scan.h"
#include "sup_serial.h"

#ifdef DEBUG_PIPELINE
#include "io.h"
#endif

#define QUEUE_SIZE 64
// ^ MUST BE POWER OF 2!

// Ring buffer for USB scancodes to report, with times (macros are the future).
static hid_event hid_queue[QUEUE_SIZE];
static uint8_t q_begin; // "pending events start" position
static uint8_t q_end; // "pending events end" - writes go to next slot.

#define BUF_NEXT(X) (X + 1) & (QUEUE_SIZE - 1)
#define BUF_EMPTY 0

static bool reports_reset_pending; // to hold report reset until macros finish.

static uint8_t pressed_codes[COMMONSENSE_MATRIX_SIZE + NUM_PEDALS];

#define CODE_EMPTY 0

#define LEAVE_TAP_MODE tap.raw = 0

#define HID_REAL_KEYDOWN HID_REAL_KEY_MASK
#define HID_REAL_KEYUP (HID_REAL_KEY_MASK | HID_RELEASED_MASK)

#define MACRO_NOT_FOUND 0

// Tap detector structure. Preferred way to check for emptiness is code == 0.
// However, comparing timestamp it's faster (and OK) to check for deadline > 0.
static union {
  struct {
    uint32_t deadline;
    uint8_t key;
    uint8_t code;
    uint16_t macro_ptr;
  } __attribute__((packed));
  uint64_t raw;
} tap;

static inline bool hid_queue_not_empty() {
  return q_begin != q_end || hid_queue[q_begin].raw != BUF_EMPTY;
}

static inline bool hid_queue_empty_at(uint8_t pos) {
  return q_begin != q_end && hid_queue[pos].raw == BUF_EMPTY;
}

static inline bool hid_queue_data_ready_at(uint8_t pos) {
  return hid_queue[pos].raw != BUF_EMPTY && hid_queue[pos].sysTime <= systime;
}

static void schedule_hid(uint32_t time, uint8_t flags, uint8_t code);

static inline bool is_special(uint8_t code) {
  // First 4 USB codes are hijacked for internal functions.
  return code < 4; // Scancode 4 is "A"
}

static inline bool is_layer_mod(uint8_t code) {
  return (code & 0xf8) == 0xa8;
}

inline uint8_t resolve_key(uint8_t key) {
  uint8_t* cell = config.layers[key];
  for (int8_t i = currentLayer; i >= 0; --i) {
    if (cell[i] == CODE_EMPTY) {
      continue;
    }
    if (config.hostMode == HM_MAC) {
      switch (cell[i]) {
        case 0xE2:
          return 0xE3;
        case 0xE3:
          return 0xE2;
        case 0xE6:
          return 0xE7;
        case 0xE7:
          return 0xE6;
        default:
          return cell[i];
      }
    } else {
      return cell[i];
    }
  }
  return CODE_EMPTY;
}


// MACRO UTILS -----------------------------------------------------------------



inline uint8_t get_macro_type(uint16_t macro_ptr) {
  return config.macros[macro_ptr] & MACRO_TYPE_MASK;
}

/*
 * Data structure: [scancode][flags][data length][macro data]
 * WARNING: the "pointer" returned is an opaque pointer-ish value.
 * ONLY compare to MACRO_NOT_FOUND and call utils - no direct use!
 */
inline uint16_t lookup_macro(
    uint8_t flags, uint8_t code, const bool find_taps) {
  uint_fast16_t ptr = 0;
  do {
#if HID_RELEASED_MASK != MACRO_TYPE_ONKEYUP
#error Please rewrite check below - it is no longer valid
#endif
    uint8_t mFlags = config.macros[ptr + 1];
    if (config.macros[ptr] == code && // obvious..
        // only keyUp macros on keyUp (tap and keyDn on keyDn)..
        ((flags ^ mFlags) & HID_RELEASED_MASK) == 0 &&
        (find_taps || (mFlags & MACRO_TYPE_TAP) == 0)) {
      return ptr + 1;
    } else {
      ptr += config.macros[ptr + 2] + 3; // length + header size
    }
  } while (ptr < sizeof config.macros &&
           config.macros[ptr] != EMPTY_FLASH_BYTE);
  return MACRO_NOT_FOUND;
}

inline uint16_t get_delay(uint8_t cmd) {
  return config.delayLib[(cmd & MACROCMD_DELAY_MASK) >> MACROCMD_DELAY_SHIFT];
}

static inline void play_macro(uint_fast16_t start) {
  --start; // externally, macro_ptr is 1-based so NULLPTR can be zero
  uint8_t *mptr = &config.macros[start] + 3;
  uint8_t *macro_end = mptr + config.macros[start + 2];
#ifdef DEBUG_PIPELINE
  ts_xprintf("Play macro: %d, sz: %d", start, config.macros[start + 2]);
#endif
  uint8_t last_keycode = 0;
  uint32_t now = systime;
  while (mptr < macro_end) {
    uint8_t cmd = *mptr;
    ++mptr;
    switch (cmd >> MACROCMD_CMD_SHIFT) {
    // Check first 2 bits - macro command
    case MACROCMD_TYPE:
      // Press+release, timing from delayLib
      // FIXME possible to queue NOEVENT here. This will most likely trigger
      // exp. header, but can be as bad as infinite loop.
      /*
      if (config.hostMode == HM_MAC && *mptr == last_keycode) {
        // IF you handle SET_IDLE - MacOS (at least on ARM) will be _mad_.
        // It will need AT LEAST 40 milliseconds to reliably detect a repeated
        // keypress (key must spend >=40ms released before it's pressed again).
        // 30ms is mostly enough, but not always.
        // Adding an extra delay before keyDown will kinda work around that - but, BUT
        // just say "FUCK No" to SET_IDLE - BLE guys strongly discourage hosts from even sending that nowadays..
        now += get_delay(DELAYS_EVENT);
      }
      */
      last_keycode = *mptr;
      schedule_hid(now, 0, *mptr);
      now += get_delay(DELAYS_EVENT); // "Type" always uses "Event delay".
      // NOTE to self: if you decide that double-delay is not needed - use get_delay(cmd) above.
      schedule_hid(now, HID_RELEASED_MASK, *mptr);
      now += get_delay(cmd); // Not strictly necessary, but just in case.
      ++mptr;
      break;
    case MACROCMD_ACTUATE:
      // Initial plans for this were to be "change modifiers". Layout to be
      // [2b (PressMod/ReleaseMod/ToggleMod/ForceMod), 4b reserved, 1B mods]
      // Currently [4b delay, 1b direction, 1b reserved, scancode]
      schedule_hid(
          now,
          (cmd & MACROCMD_ACTUATE_KEYUP) ? HID_RELEASED_MASK : 0,
          *mptr);
      now += get_delay(cmd);
      ++mptr;
      break;
    case MACROCMD_IGNORED:
      // 2 bits - PushMods, PopMods, RevertMods. Not used currently.
      break;
    case MACROCMD_WAIT:
      now += get_delay(cmd);
      break;
    default:
      return;
    }
  }
}



// ---------------------------------------------------------------- /MACRO UTILS



static inline void process_layerMods(uint8_t flags, uint8_t code) {
  // codes A8-AB - momentary selection(Fn), AC-AF - permanent(LLck)
  if (code & 0x04) {
    // LLck. Keydown flips the bit, keyup is ignored.
    if (flags & KEY_UP_MASK) {
      return;
    }
    FLIP_BIT(layerMods, (code & 0x03) + LAYER_MODS_SHIFT);
  } else if ((flags & KEY_UP_MASK) == 0) {
    // Fn Press
    SET_BIT(layerMods, (code & 0x03) + LAYER_MODS_SHIFT);
  } else {
    // Fn Release
    CLEAR_BIT(layerMods, (code & 0x03) + LAYER_MODS_SHIFT);
  }
  uint8_t oldLayer = currentLayer;
  // Figure layer condition
  for (uint8_t i = 0; i < sizeof(config.layerConditions); i++) {
    if (layerMods == (config.layerConditions[i] & 0xf0)) {
      currentLayer = config.layerConditions[i] & 0x0f;
      break;
    }
  }
#ifdef DEBUG_PIPELINE
  ts_xprintf("LM: %02x %02x: L%d->%d", flags, code, oldLayer, currentLayer);
#endif
  if (oldLayer == currentLayer) {
    return;
  }
#ifdef REEVALUATE_ON_LAYER_CHANGE
  // OK. Now we have to see which keys are pressed, and what will change.
  for (uint8_t keyIndex = 0; keyIndex < sizeof pressed_codes; ++keyIndex) {
    uint8_t old_code = pressed_codes[keyIndex];
    if (old_code == CODE_EMPTY) { // Key isn't pressed
      continue; // NO key should map to CODE_EMPTY below real codes.
    }
    if (is_layer_mod(old_code)) {
      continue; // Avoiding loops from releasing the layer mod key.
    }
    uint8_t new_code = resolve_key(keyIndex); // Reevaluate on new layer
    if (new_code == old_code) {
      continue; // no change..
    }
    if (is_layer_mod(new_code)) {
      continue; // Avoiding loops from pressing the layer mod from new layer.
    }
    // Re-push key for the user.
#ifdef DEBUG_PIPELINE
    ts_xprintf("LM remap %d: %d -> %d", keyIndex, old_code, new_code);
#endif
    scan_register_event(KEY_UP_MASK, keyIndex);
    scan_register_event(0, keyIndex);
  }
#endif
}

static inline void process_real_key(void) {
  if (tap.deadline > 0 && systime > tap.deadline) {
#ifdef DEBUG_PIPELINE
    ts_xprintf("Tap: macro@%d timed out", tap.macro_ptr);
#endif
    // Tap timeout. MAKE DOUBLE SURE this branch is tap timeout situation ONLY.
    // See <TapWait> section for details.
    tap.macro_ptr = MACRO_NOT_FOUND;
  } else if (reports_reset_pending) {
    // It is possible to get stuck here for LONG TIME if user makes something
    // stupid (namely, long macro with huge delays), triggers it and then
    // releases all keys generating a pending report reset.
    // Scancodes will be generated, but not processed until _ALL_ USB events
    // generated by that macro are dispatched to host. Scancodes ring buffer
    // can overflow - not causing memory corruption, but losing some events.
    // Which _is_ bad, don't get me wrong - but user had it coming.
    if (hid_queue_not_empty()) {
      return;
    }
#ifdef DEBUG_PIPELINE
    ts_xprintf("Reset: commencing, data %d - %d", q_begin, q_end);
#endif

    hid_reset_reports();
    serial_reset_reports();
    reports_reset_pending = false;
  }


  scan_event_t event = scan_read_event();
  uint8_t code = 0;
  if (event.key != COMMONSENSE_NOKEY) {
    if (event.flags & KEY_UP_MASK) {
      // Resolved value may differ due to layer changes. Using saved value.
      code = pressed_codes[event.key];
      pressed_codes[event.key] = CODE_EMPTY;
    } else {
      code = resolve_key(event.key);
      pressed_codes[event.key] = code;
    }
    if (code == CODE_EMPTY) {
      // Empty key in layout from current layer down to base. DON'T EVER.
      return;
    }
    if (is_layer_mod(code)) {
      // Layer mod. The "layer mod key not mapped in upper layers" solved
      // by key resolver - as long as upper layers not mapped Fn key to
      // something else - which is clowny and should be punished anyway.
      process_layerMods(event.flags, code);
      return;
    }
  } else {
    // An empty value. Likely because nothing was pressed last tick, but..
    if (event.flags & KEY_UP_MASK) {
      // This is "All keys are up" signal, sun keyboard-style. It deals with
      // stuck keys. Those appear due to layers - suppose you press the key,
      // then switch layer which has another key at that scancode position.
      // When you release the key, non-existent key release is generated -
      // which is not that bad - but first key is stuck forever.
      reports_reset_pending = true;
#ifdef DEBUG_PIPELINE
      ts_xprintf("Reset: pending");
#endif
    }
    if (tap.code == 0 || tap.macro_ptr != MACRO_NOT_FOUND) {
      return; // 1) Not TapWait, 2) not TapWait timeout
    }
  }
#ifdef DEBUG_PIPELINE
  coded_message_t cm;
  cm.messageCode = MC_KEY_RESOLVED;
  mc_key_resolved_payload_t* msg = (mc_key_resolved_payload_t*)&cm.message;
  msg->key = event.key;
  msg->flags = event.flags;
  msg->layer = currentLayer;
  msg->code = code;
  coded_timestamped_message(&cm);
#endif

  uint8_t keyflags = event.flags | HID_REAL_KEY_MASK;
  if (tap.code > 0) { // <TapWait>
    // Tap wait mode. We are here because tap macro triggered in the past.
    if (code == tap.code && systime <= tap.deadline) {
      // Ok, the key matches quickly enough. Play macro, resume normal mode.
#ifdef DEBUG_PIPELINE
      ts_xprintf("Tap: macro@%d", tap.macro_ptr);
#endif
      play_macro(tap.macro_ptr);
      LEAVE_TAP_MODE;
      return; // Eat the keyUp by not queueing it.
    }
    // Ok, tap not happened. Another key, not quick enough - NOT IMPORTANT.
    // keyUp will be processed in main/normal mode branch below - NOT SPECIAL.

    // keyDown macro on that key, though, _is_ special - we need to trigger it
    // _instead_of_ tap - and then process current keypress in a normal way.
    uint16_t keyDown_ptr = lookup_macro(HID_REAL_KEYDOWN, tap.code, false);
    if (keyDown_ptr != MACRO_NOT_FOUND) {
      // Play the keyDown, play it again, my Johnny..
      play_macro(keyDown_ptr);
    } else {
      // Ugh.. no macro.. just pretend we pressed the key back then.
      // Post-dating the event to ensure keypress is sent to USB _this_ tick -
      // possibly postponing the scheduled keypresses of any macros.
      schedule_hid(systime - 120, HID_REAL_KEY_MASK, tap.code);
    }
    if (tap.macro_ptr == MACRO_NOT_FOUND) {
      LEAVE_TAP_MODE; // Not above because this macro clears whole tap struct
      // Switch to normal mode. Early return - no real keys were involved.
      return;
    }
    LEAVE_TAP_MODE; // ..still need to process the actual key event, continue
  } // </TapWait>
  // Trick: FlightController must save tap macros before keyDown macros - so
  // if both defined, we find tap first.
  uint16_t macro_ptr = lookup_macro(keyflags, code, true);
  if (macro_ptr != MACRO_NOT_FOUND) {
    uint8_t macro_type = get_macro_type(macro_ptr);
    if (macro_type == MACRO_TYPE_TAP) {
      // Enter the TapWait mode.
      tap.macro_ptr = macro_ptr;
      tap.key = event.key;
      tap.code = code;
      tap.deadline = systime + config.delayLib[DELAYS_TAP];
      return;
    }
    play_macro(macro_ptr);
    // Replace triggering event with macro by eating the original event.
    if (macro_type != MACRO_TYPE_ONKEYUP) {
      // Except for keyUp macro - the trigger key is already down, must release!
      return;
    }
  }
  // OK. Not macro.
  // NOTE: schedule_hid skips non-zero cells in the buffer. Think about overruns
  // NOTE2: we still want to maintain order? Otherwise linked list is probably
  // better (though expensive at 4B/pointer plus memory management)
  schedule_hid(systime, keyflags, code);
}

static inline void schedule_hid(uint32_t time, uint8_t flags, uint8_t code) {
  uint8_t pos = q_end;
  do {
    // TODO think how not to fall into infinite loop here if buffer is full.
    pos = BUF_NEXT(pos);
  } while (hid_queue[pos].raw != BUF_EMPTY);
#ifdef DEBUG_PIPELINE
  coded_message_t cm;
  cm.messageCode = MC_SCHEDULE_HID;
  mc_schedule_hid_payload_t* msg = (mc_schedule_hid_payload_t*)&cm.message;
  msg->event_time = time;
  msg->code = code;
  msg->flags = flags;
  msg->position = pos;
  msg->data_begin = q_begin;
  msg->data_end = q_end;
  coded_timestamped_message(&cm);
#endif
  // Special codes - they're not queued, but processed RIGHT NOW.
  // Not sure macro-generated keys should be processed, but right now they are..
  if (is_special(code)) {
    if (flags & HID_RELEASED_MASK) {
      return; // keyUp is ignored so not to toggle twice.
    }
    switch (code) {
      case 2:
        Boot_Load();
        break; // NORETURN function, break is strictly for consistence.
      case 3:
        gpio_toggle_expHdr();
        break;
      default:
        break;
    }
    return;
  } else if (is_layer_mod(code)) {
    // Not valid here - layer mods must be physical keys.
    ts_xprintf("CLOWNTOWN Layer mod scheduled as synthetic key!");
    return;
  }
  hid_queue[pos].sysTime = time;
  hid_queue[pos].flags = flags;
  hid_queue[pos].code = code;
  q_end = pos;
}

/*
 * Due to macros there might be future AND out-of-order scancodes.
 * Main Idea: pin readpos until scancode under it is retired.
 * So, rpos to wpos would be the working area containing pending scancodes.
 * This can (and, in macros world, will) lead to gaps in the working area.
 * This Is Fine.
 * Current implementation just eats up all the empty positions before starting,
 * so technically nothing is pinned, it's just sparkling garbage collection.

 * TODO: implement out of order key release?
 * NOTE ^ very rare situations where this is needed.
 */
static inline void update_reports(void) {
  if (cooldown_timer > 0) {
    // Slow down! Delay 0 controls update rate.
    // we should be called every millisecond - so setting delay0 to 10 will
    // essentially makes it 100Hz keyboard with latency of 1kHz one.
    cooldown_timer--;
    return;
  }
  // Eating up all the processed scancodes
  while (hid_queue_empty_at(q_begin)) {
    q_begin = BUF_NEXT(q_begin);
  }
  // We might have hit wpos here already. Not much point specialcasing tho -
  // there might be an actionable scancode at the very tail..

  uint8_t pos = q_begin;
  while (pos != q_end && !hid_queue_data_ready_at(pos)) {
    pos = BUF_NEXT(pos); // Not consuming - just peeking!
  }
  if (!hid_queue_data_ready_at(pos)) {
    // Can only fail at rpos == wpos AKA "all events are in the future"
    return; // We'll return here on the next tick, hopefully passing.
  }
#ifdef DEBUG_PIPELINE
  coded_message_t cm;
  cm.messageCode = MC_PROCESS_HID;
  mc_process_hid_payload_t* msg = (mc_process_hid_payload_t*)&cm.message;
  msg->event_time = hid_queue[pos].sysTime;
  msg->code = hid_queue[pos].code;
  msg->flags = hid_queue[pos].flags;
  msg->position = pos;
  msg->data_begin = q_begin;
  msg->data_end = q_end;
  coded_timestamped_message(&cm);
#endif
  if (is_special(hid_queue[pos].code)) {
    // Clowntown: fully "transparent" will toggle exp. header
    // But it should not ever be put on queue!
    gpio_toggle_expHdr();
    hid_queue[pos].flags |= HID_RELEASED_MASK; // skip-cooldown trick.
    // ALL codes you want filtered from reports MUST BE ABOVE THIS LINE!
  } else if (hid_queue[pos].code >= 0xe8) {
    hid_update_consumer(&hid_queue[pos]);
  } else if (hid_queue[pos].code >= 0xa5 &&
             hid_queue[pos].code <= 0xa7) {
    hid_update_system(&hid_queue[pos]);
  } else {
    switch (output_direction) {
      case OUTPUT_DIRECTION_USB:
        hid_update_keyboard(&hid_queue[pos]);
        break;
      case OUTPUT_DIRECTION_SERIAL:
        update_serial_keyboard_state(&hid_queue[pos]);
        break;
      default:
        break;
    }
  }
  if ((hid_queue[pos].flags & HID_RELEASED_MASK) == 0) {
    // We only throttle keypresses. Key release doesn't slow us down -
    // minimum duration is guaranteed by fact that key release goes after
    // key press and keypress triggers cooldown.
    cooldown_timer = config.delayLib[DELAYS_EVENT];
    gpio_keypress(hid_queue[pos].code); // allow ExpHdr to see the scancode
  }
  hid_queue[pos].raw = BUF_EMPTY;
  // Done. Will bump .begin next cycle next cycle - to avoid .end > .begin
}

inline void pipeline_process(void) {
  if (false) {
    /*
            playing non-game macro
                if wait is blocking:
                    if SCB has macro SC release:
                        set that SCB pos to 0
                        play after-wait part of macro to USBCB w/proper
       timestamps
    */
  } else {
    process_real_key();
  }
  update_reports();
}

inline bool pipeline_process_wakeup(void) {
  scan_event_t event = scan_read_event();
  // We don't care about _which_ key is pressed - we wake up on key _press_
  return (event.raw != scan_no_key.raw) && (event.flags & KEY_UP_MASK) == 0;
}

void pipeline_init(void) {
  // Pipeline is worked on in main loop only, no point disabling IRQs to avoid
  // preemption.
  scan_reset();
  hid_init();
  q_begin = 0;
  q_end = 0;
  cooldown_timer = 0;
  for (uint8_t i = 0; i < QUEUE_SIZE; ++i) {
    hid_queue[i].raw = BUF_EMPTY;
  }
  for (uint8_t i = 0; i < sizeof pressed_codes; ++i) {
    pressed_codes[i] = CODE_EMPTY;
  }
  LEAVE_TAP_MODE;
  reports_reset_pending = false;
}

#undef CODE_EMPTY

#undef LEAVE_TAP_MODE

#undef QUEUE_SIZE
#undef BUF_EMPTY
#undef BUF_NEXT

#undef MACRO_NOT_FOUND
#undef HID_REAL_KEYDOWN
#undef HID_REAL_KEYUP
