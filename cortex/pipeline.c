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

extern const scan_event_t scan_no_key;

inline bool empty_keycode_at(uint8_t pos) {
  return USBQueue[pos].keycode == USBCODE_NOEVENT;
}

inline bool valid_actionable_keycode_at(uint8_t pos) {
  return USBQueue[pos].keycode != USBCODE_NOEVENT &&
      USBQueue[pos].sysTime <= systime;
}

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
  // Aktchually figure out what's pressed and how it should change with layer change
  // then enqueue DIFFS ONLY
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
    // Special keycodes - see HID docs. Hijacked for Nefarious Purposes.
    // These keys are special - they CANNOT be used as macro triggers.
    if (flags & USBQUEUE_RELEASED_MASK) {
      return; // keyUp is ignored so not to toggle twice.
    }
    switch (keycode) {
      case USBCODE_BOOTLDR:
        Boot_Load(); // Does not return, no need for break
      case USBCODE_EXP_TOGGLE:
        exp_toggle();
        break;
      default:
        break;
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
  while (!empty_keycode_at(USBQueue_wpos)) {
    KEYCODE_BUFFER_STEP(USBQueue_wpos);
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

  scan_event_t event = scan_read_event();  
  if (event.key == COMMONSENSE_NOKEY) {
    // An empty value. Likely because nothing was pressed last tick, but..
    if (event.flags & KEY_UP_MASK) {
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
  // OK, we're done with special cases. General scancode processing starts here.

  // Resolve USB keycode using current active layers - drop down until defined.
  uint8_t usb_sc = USBCODE_TRANSPARENT;
  for (int8_t i = currentLayer; i >= 0; --i) {
    usb_sc = config.layers[i][event.key];
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

  uint8_t keyflags = event.flags | USBQUEUE_REAL_KEY_MASK;
  uint_fast16_t macro_ptr;
  if (tap_deadline > 0) { // <TapWait>
    // Tap wait mode. We are here because tap macro triggered on previous tick.
    if (usb_sc == tap_usb_sc || event.key == COMMONSENSE_NOKEY) {
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
inline void update_reports(void) {
  if (cooldown_timer > 0) {
    // Slow down! Delay 0 controls update rate.
    // we should be called every millisecond - so setting delay0 to 10 will
    // essentially makes it 100Hz keyboard with latency of 1kHz one.
    cooldown_timer--;
    return;
  }
  // Eating up all the processed scancodes
  while (USBQueue_rpos != USBQueue_wpos && empty_keycode_at(USBQueue_rpos)) {
    KEYCODE_BUFFER_STEP(USBQueue_rpos); // Consuming real buffer!
  }
  // We might have hit wpos here already. Not much point specialcasing tho -
  // there might be an actionable scancode at the very tail..

  uint8_t pos = USBQueue_rpos;
  while (pos != USBQueue_wpos && !valid_actionable_keycode_at(pos)) {
    KEYCODE_BUFFER_STEP(pos); // Not consuming - just peeking!
  }
  if (!valid_actionable_keycode_at(pos)) { // Can only fail at rpos == wpos
    return; // We'll return here on the next tick, hopefully passing.
  }

  if (USBQueue[pos].keycode < USBCODE_A) {
    // Clowntown: fully "transparent" will toggle exp. header
    // But it should not ever be put on queue!
    exp_toggle();
    USBQueue[pos].flags |= USBQUEUE_RELEASED_MASK; // skip-cooldown trick.
    // ALL codes you want filtered from reports MUST BE ABOVE THIS LINE!
  } else if (USBQueue[pos].keycode >= 0xe8) {
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
    cooldown_timer = config.delayLib[DELAYS_EVENT];
    exp_keypress(USBQueue[pos].keycode); // allow ExpHdr to see the scancode
  }
  USBQueue[pos].keycode = USBCODE_NOEVENT; // Done, rpos will move next cycle.
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
  USBQueue_rpos = 0;
  USBQueue_wpos = 0;
  cooldown_timer = 0;
  memset(USBQueue, USBCODE_NOEVENT, sizeof USBQueue);
  saved_macro_ptr = MACRO_NOT_FOUND;
}
