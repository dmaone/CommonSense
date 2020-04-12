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

#include "exp.h"
#include "scan.h"
#include "PSoC_USB.h"
#include "sup_serial.h"

uint8_t tap_usb_sc;
uint32_t tap_deadline;
uint_fast16_t saved_macro_ptr;

inline void process_layerMods(uint8_t flags, uint8_t keycode) {
  // codes A8-AB - momentary selection(Fn), AC-AF - permanent(LLck)
  if (keycode & 0x04) {
    // LLck. Keydown flips the bit, keyup is ignored.
    if (flags & KEY_UP_MASK) {
      return;
    }
    FLIP_BIT(layerMods, (keycode & 0x03) + LAYER_MODS_SHIFT);
  } else if ((flags & KEY_UP_MASK) == 0) {
    // Fn Press
    SET_BIT(layerMods, (keycode & 0x03) + LAYER_MODS_SHIFT);
  } else {
    // Fn Release
    CLEAR_BIT(layerMods, (keycode & 0x03) + LAYER_MODS_SHIFT);
  }
  // Figure layer condition
  for (uint8_t i = 0; i < sizeof(config.layerConditions); i++) {
    if (layerMods == (config.layerConditions[i] & 0xf0)) {
      currentLayer = config.layerConditions[i] & 0x0f;
      break;
    }
  }
#ifdef DEBUG_PIPELINE
  xprintf("L@%d: %02x %02x -> %d", systime, flags, keycode, currentLayer);
#endif
}

/*
 * Data structure: [scancode][flags][data length][macro data]
 */
inline uint_fast16_t lookup_macro(uint8_t flags, uint8_t keycode) {
  uint_fast16_t ptr = 0;
  do {
#if USBQUEUE_RELEASED_MASK != MACRO_TYPE_ONKEYUP
#error Please rewrite check below - it is no longer valid
#endif
    uint8_t mFlags = config.macros[ptr + 1];
    if (config.macros[ptr] == keycode && // obvious..
        // only keyUp macros on keyUp (tap and keyDn on keyDn)..
        ((flags ^ mFlags) & USBQUEUE_RELEASED_MASK) == 0 &&
        // in tap wait, skip tap macros. See TapWait processing for why.
        (tap_deadline == 0 || (mFlags & MACRO_TYPE_TAP) == 0)) {
      return ptr;
    } else {
      ptr += config.macros[ptr + 2] + 3; // length + header size
    }
  } while (ptr < sizeof config.macros &&
           config.macros[ptr] != EMPTY_FLASH_BYTE);
  return MACRO_NOT_FOUND;
}

inline void queue_usbcode(uint32_t time, uint8_t flags, uint8_t keycode) {
#ifdef DEBUG_PIPELINE
  xprintf("Q@%d: %02x %d @%+d", systime, flags, keycode, time - systime);
#endif
  // Special keycodes - they're not queued, but processed RIGHT NOW.
  // Not sure macro-generated keys should be processed, but right now they are..
  if (keycode < USBCODE_A) {
    // Special keycodes - see HID docs.
    // Hijacked for Nefarious Purposes - but only ExpToggle is used for now.
    // These keys are special - they CANNOT be used as macro triggers.
    if (keycode == USBCODE_EXP_TOGGLE  && !(flags & USBQUEUE_RELEASED_MASK)) {
      // Exp header toggles on keyDown. keyUp is ignored so not to toggle twice.
      exp_toggle();
    }
    return;
  } else if ((keycode & 0xf8) == 0xa8) {
    // Layer mod. The "layer mod key not mapped in upper layers" solved
    // by USB keycode lookup, as long as upper layers not mapped Fn key to
    // something else - which is clowny and should be punished anyway.
    process_layerMods(flags, keycode);
    return;
  }
  // Trick - even if current buffer position is empty, write to the next one.
  // It may be empty because reader already processed it.
  USBQueue_wpos = KEYCODE_BUFFER_NEXT(USBQueue_wpos);
  // Make sure not to overwrite pending events - TODO think how not to fall into
  // infinite loop if buffer full.
  while (USBQueue[USBQueue_wpos].keycode != USBCODE_NOEVENT) {
    USBQueue_wpos = KEYCODE_BUFFER_NEXT(USBQueue_wpos);
  }
  USBQueue[USBQueue_wpos].sysTime = time;
  USBQueue[USBQueue_wpos].flags = flags;
  USBQueue[USBQueue_wpos].keycode = keycode;
}

inline void play_macro(uint_fast16_t start) {
  uint8_t *mptr = &config.macros[start] + 3;
  uint8_t *macro_end = mptr + config.macros[start + 2];
#ifdef DEBUG_PIPELINE
  xprintf("PM@%d: %d, sz: %d", systime, start, config.macros[start + 2]);
#endif
  uint32_t now = systime;
  uint_fast16_t delay;
  uint8_t keyflags;
  while (mptr <= macro_end) {
    delay = config.delayLib[(*mptr >> 2) & 0x0f];
    switch (*mptr >> 6) {
    // Check first 2 bits - macro command
    case 0: // TypeOneKey
      // Press+release, timing from delayLib
      mptr++;
      // FIXME possible to queue USB_NOEVENT.
      // This will most likely trigger exp. header, but may be as bad as
      // infinite loop.
      queue_usbcode(now, 0, *mptr);
      now += delay;
      queue_usbcode(now, USBQUEUE_RELEASED_MASK, *mptr);
      break;
    case 1: // ChangeMods - currently PressKey and ReleaseKey
      /*
       * Initial plans for this were to be "change modifiers". Binary format - 
       * 2 bits for (PressMod/ReleaseMod/ToggleMod/ForceMod)
       * then 4 bits not used, then a byte of standard modflags.
       * Now, though, it's 
       * [4 bits delay, 1 bit direction, 1 bit reserved, 1 byte scancode]
       */
      keyflags =
          (*mptr & MACRO_KEY_UPDOWN_RELEASE) ? USBQUEUE_RELEASED_MASK : 0;
      mptr++;
      queue_usbcode(now, keyflags, *mptr);
      now += delay;
      break;
    case 2: // Mods stack manipulation
      // 2 bits - PushMods, PopMods, RevertMods.
      // Not used currently.
      break;
    case 3: // Wait
      /* 4 bits for delay. Initially had special value of 
       * 0 = "Wait for trigger key release", others from delayLib.
       * But since now we have separate triggers on press and release - all
       * values are from delayLib.
       */
      now += delay;
      break;
    default:
      return;
    }
    mptr++;
  }
}

inline scancode_t read_scancode(void) {
  if (scancodes_rpos == scancodes_wpos) {
    // Nothing to read. Return empty value AKA "Pressed nokey".
    scancode_t result;
    result.flags = 0;
    result.scancode = COMMONSENSE_NOKEY;
    return result;
  }
  // Skip empty elements that might be there
  while (scancodes[scancodes_rpos].scancode == COMMONSENSE_NOKEY &&
        (scancodes[scancodes_rpos].flags & USBQUEUE_RELEASED_MASK) == 0
  ) {
    scancodes_rpos = SCANCODES_NEXT(scancodes_rpos);
  }
  // MOVE the value from ring buffer to output buffer. Mark source as empty.
  scancode_t scancode = scancodes[scancodes_rpos];
  scancodes[scancodes_rpos].flags = 0;
  scancodes[scancodes_rpos].scancode = COMMONSENSE_NOKEY;
#ifdef MATRIX_LEVELS_DEBUG
  xprintf("sc: %d %d @ %d ms, lvl %d/%d", scancode & KEY_UP_MASK,
          scancode, systime,
          level_buffer[scancodes_rpos],
          level_buffer_inst[scancodes_rpos]);
#endif
  return scancode;
}

bool reports_reset_pending;
inline void process_real_key(void) {
  if (tap_deadline > 0 && systime > tap_deadline) {
    // Tap timeout. MAKE DOUBLE SURE this is a timeout situation.
    // Clownetowne: we need to switch from TapWait to normal mode and send that
    // keyDown. So we send ANOTHER keyDown for the trigger key, but let our
    // later self know it's actually a timeout.
#ifdef DEBUG_PIPELINE
    xprintf("Tap@%d: %d timed out", systime, saved_macro_ptr);
#endif
    saved_macro_ptr = MACRO_NOT_FOUND; // Large-ish footgun!
  } else if (reports_reset_pending) {
    if (USBQUEUE_IS_EMPTY) {
      // ONLY reset if there are no pending USB events.
      // Those are supposed to only be generated by macros, but who knows.
      reset_reports();
      serial_reset_reports();
      reports_reset_pending = false;
    }
    if (reports_reset_pending) {
      // It is possible to get stuck here for LONG TIME if user makes something
      // stupid (namely, long macro with huge delays), triggers it and releases
      // all keys (see next big comment why that's important).
      // Scancodes will be generated, but not processed until _ALL_ USB events
      // generated by that macro are dispatched to host. Scancodes ring buffer
      // can overflow - not causing memory corruption, but losing some events.
      // Which _is_ bad, don't get me wrong - but user had it coming.
      return;
    }
  }
  scancode_t sc = read_scancode();
  if (sc.scancode == COMMONSENSE_NOKEY) {
    // An empty value. Likely because nothing was pressed last tick, but..
    if (sc.flags & KEY_UP_MASK) {
      // This is "All keys are up" signal, sun keyboard-style. It deals with
      // stuck keys. Those appear due to layers - suppose you press the key,
      // then switch layer which has another key at that scancode position.
      // When you release the key, non-existent key release is generated -
      // which is not that bad - but first key is stuck forever.
      reports_reset_pending = true;
    }
    if (tap_deadline == 0 || saved_macro_ptr != MACRO_NOT_FOUND) {
      // not TapWait, not TapWait timeout either, no key.. Done!
      return;
    }
  }
  if (TEST_BIT(status_register, C2DEVSTATUS_SETUP_MODE)) {
    // In setup mode all scancodes(not USB!) go up the control channel, not HID.
    outbox.response_type = C2RESPONSE_SCANCODE;
    outbox.payload[0] = sc.flags;
    outbox.payload[1] = sc.scancode;
    usb_send_c2();
    return;
  }

  // OK, we're done with special cases. General scancode processing starts here.

  // Resolve USB keycode using current active layers - drop down until defined.
  uint8_t usb_sc = USBCODE_TRANSPARENT;
  for (int8_t i = currentLayer; i >= 0; --i) {
    usb_sc = config.layers[i][sc.scancode];
#ifdef DEBUG_PIPELINE
    xprintf("Lookup@%d: %02x %02x@L%d -> %d",
            systime, sc.flags, sc.scancode, i, usb_sc);
#endif
    if (usb_sc != USBCODE_TRANSPARENT) {
      break;
    }
  }
  if (usb_sc == USBCODE_TRANSPARENT) {
    // Empty key in layout from current layer down to base. Fuck no, DON'T EVER.
    return;
  }
#ifdef DEBUG_PIPELINE
  xprintf("SC@%d: %02x %02x@L%d -> %d",
          systime, sc.flags, sc.scancode, currentLayer, usb_sc);
#endif

  uint8_t keyflags = sc.flags | USBQUEUE_REAL_KEY_MASK;
  uint_fast16_t macro_ptr;
  if (tap_deadline > 0) { // <TapWait>
    // Tap wait mode. We are here because tap macro triggered on previous tick.
    if (usb_sc == tap_usb_sc || sc.scancode == COMMONSENSE_NOKEY) {
      // Ok, the key matches.
      // Or it's a tap timeout - in which case we KIND OF have a second keyDown.
      if (systime <= tap_deadline) {
        // Quick enough. Play macro, eat keyUp, return to normal mode.
#ifdef DEBUG_PIPELINE
        xprintf("Tap@%d: %d", systime, saved_macro_ptr);
#endif
        play_macro(saved_macro_ptr);
        tap_deadline = 0;
        return; // Eat the keyUp by not queueing it.
      }
    }
    // Ok, NOT a tap. Doesn't matter why, really. BUT there COULD HAVE BEEN
    // a keyDown macro on that key. keyUp, too - but that we will process in a
    // business-as-usual way.

    // Flags override has a clowntown potential, I admit.
    // Should probably save it from the keypress time, but naaah, wcgw.
    // Also macro lookup is Special - it skips tap macros in TapWait mode.
    macro_ptr = lookup_macro(USBQUEUE_REAL_KEY_MASK, tap_usb_sc);
    if (macro_ptr == MACRO_NOT_FOUND) {
      // Ugh.. Less-clowny way, just pretend we submitted the key a bit ago
      queue_usbcode(tap_deadline - 120, USBQUEUE_REAL_KEY_MASK, tap_usb_sc);
    } else {
      // Play the keyDown, play it again, my Johnny..
      play_macro(macro_ptr);
    }
    // And now back to normal processing of what's hopefully a keyDown.
    tap_deadline = 0;
    if (saved_macro_ptr == MACRO_NOT_FOUND) {
      // Stop all the madness // stop clowntown // it's my life (c) Dr. Alban.
      // Seriously though - this is special mode for TapWait timeout.
      // There is no real key. Don't complicate downstream's life.
      return;
    }
  } // </TapWait>
  // Trick: FlightController must save tap macros before keyDown macros - so
  // if both defined, we find tap first.
  macro_ptr = lookup_macro(keyflags, usb_sc);
  if (macro_ptr != MACRO_NOT_FOUND) {
    if ((config.macros[macro_ptr + 1] & MACRO_TYPE_TAP) == 0) {
      play_macro(macro_ptr);
    } else {
      // Tap macro cannot be selected for keyUp. So this must be keyDown.
      // Enter the TapWait mode.
      saved_macro_ptr = macro_ptr;
      tap_usb_sc = usb_sc;
      tap_deadline = systime + config.delayLib[DELAYS_TAP];
    }
    return;
  }
  // OK. Not macro.
  // NOTE: queue_usbcode skips non-zero cells in the buffer. Think what to do
  // on overflow.
  // NOTE2: we still want to maintain order? Otherwise linked list is probably
  // better (though expensive at 4B/pointer plus memory management)
  queue_usbcode(systime, keyflags, usb_sc);
}

#define NO_COOLDOWN USBQueue[pos].flags |= USBQUEUE_RELEASED_MASK;
/*
    Idea: not move readpos until keycode after it is processed.
    So, readpos to writepos would be the working area.

    TODO: maintain bitmap of currently pressed keys to release them on reset and
   for better KRO handling.
    TODO: implement out of order key release?
    NOTE ^ ^^ very rare situations where this is needed.
 */
inline void update_reports(void) {
  if (cooldown_timer > 0) {
    // Slow down! Delay 0 controls update rate.
    // we should be called every millisecond - so setting delay0 to 10 will
    // essentially makes it 100Hz keyboard with latency of 1kHz one.
    cooldown_timer--;
    return;
  }
  if (USBQUEUE_IS_EMPTY) {
    return;
  }
  // If there's change - find first non-empty buffer cell.
  // This results in infinite loop on empty buffer - must take care not to queue
  // NOEVENTs.
  while (USBQueue[USBQueue_rpos].keycode == USBCODE_NOEVENT) {
    USBQueue_rpos = KEYCODE_BUFFER_NEXT(USBQueue_rpos);
  }
  // xprintf("USB queue %d - %d", USBQueue_rpos, USBQueue_wpos);
  uint8_t pos = USBQueue_rpos;
  do {
    if (USBQueue[pos].keycode != USBCODE_NOEVENT &&
        USBQueue[pos].sysTime <= systime) {
      if (USBQueue[pos].keycode < USBCODE_A) {
        // side effect - key transparent till the bottom will toggle exp. header
        // But it should not ever be put on queue!
        exp_toggle();
        NO_COOLDOWN
      }
      // Codes you want filtered from reports MUST BE ABOVE THIS LINE!
      // -> Think of special code for collectively settings mods!
      else if (USBQueue[pos].keycode >= 0xe8) {
        update_consumer_report(&USBQueue[pos]);
      } else if (USBQueue[pos].keycode >= 0xa5 &&
                 USBQueue[pos].keycode <= 0xa7) {
        update_system_report(&USBQueue[pos]);
      } else {
        switch (output_direction) {
          case OUTPUT_DIRECTION_USB:
            update_keyboard_report(&USBQueue[pos]);
            break;
          case OUTPUT_DIRECTION_SERIAL:
            update_serial_keyboard_report(&USBQueue[pos]);
            break;
          default:
            break;
        }

      }
      if ((USBQueue[pos].flags & USBQUEUE_RELEASED_MASK) == 0) {
        // We only throttle keypresses. Key release doesn't slow us down -
        // minimum duration is guaranteed by fact that key release goes after
        // key press and keypress triggers cooldown.
        cooldown_timer = config.delayLib[DELAYS_EVENT]; // Actual update
                                                        // happened - reset
                                                        // cooldown.
        exp_keypress(
            USBQueue[pos].keycode); // Let the downstream filter by keycode
      }
      USBQueue[pos].keycode = USBCODE_NOEVENT;
      if (pos == USBQueue_rpos && pos != USBQueue_wpos) {
        // Only if we're at first position. Previous cells may contain future
        // actions otherwise! Also if not the last item - we don't want to
        // overrun the buffer.
        USBQueue_rpos = KEYCODE_BUFFER_NEXT(pos);
      }
      break;
    }
  } while (pos != USBQueue_wpos);
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
  scancode_t sc = read_scancode();
  // We don't care about _which_ key is pressed - we wake up on key _press_
  return sc.scancode != COMMONSENSE_NOKEY && (sc.flags & KEY_UP_MASK) == 0;
}

void pipeline_init(void) {
  // Pipeline is worked on in main loop only, no point disabling IRQs to avoid
  // preemption.
  scan_reset();
  USBQueue_rpos = 0;
  USBQueue_wpos = 0;
  cooldown_timer = 0;
  memset(USBQueue, USBCODE_NOEVENT, sizeof USBQueue);
  saved_macro_ptr = MACRO_NOT_FOUND;
}
