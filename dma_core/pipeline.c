/*
 *
 * Copyright (C) 2016-2017 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
*/
#include <string.h>
#include <project.h>
#include "pipeline.h"
#include "scan.h"
#include "PSoC_USB.h"
#include "exp.h"

uint8_t pipeline_prev_usbkey;
uint32_t pipeline_prev_usbkey_time;

inline void process_layerMods(uint8_t sc, uint8_t keycode)
{
    // codes A8-AB - momentary selection, AC-AF - permanent
    if (keycode & 0x04)
    {
        if ((sc & KEY_UP_MASK) == 0) {
            // Press
            currentLayer = keycode & 0x03;
        }
        // Release is ignored
    } else {
        if ((sc & KEY_UP_MASK) == 0) {
            // Press
            layerMods |= (1 << ((keycode & 0x03) + LAYER_MODS_SHIFT));
        } else {
            // Release
            layerMods &= ~(1 << ((keycode & 0x03) + LAYER_MODS_SHIFT));
        }
        // Figure layer condition
        for (uint8_t i=0; i < sizeof(config.layerConditions); i++)
        {
            if (layerMods == (config.layerConditions[i] & 0xf0))
            {
                currentLayer = (config.layerConditions[i] & 0x0f);
                break;
            }
        }
    }
}

/*
 * Data structure: [scancode][flags][data length][macro data]
 */
inline uint_fast16_t lookup_macro(uint8_t flags, uint8_t keycode)
{
    uint_fast16_t ptr = 0;
    do
    {
#if USBQUEUE_RELEASED_MASK != MACRO_TYPE_ONKEYUP
#error Please rewrite check below - it is no longer valid
#endif
        if (
            config.macros[ptr] == keycode
         && (flags & USBQUEUE_RELEASED_MASK) == (config.macros[ptr+1] & MACRO_TYPE_ONKEYUP)
        )
        {
            return ptr;
        }
        else
        {
            ptr += config.macros[ptr+2] + 3;
        }
    } while ( ptr < sizeof config.macros && config.macros[ptr] != EMPTY_FLASH_BYTE );
    return MACRO_NOT_FOUND;
}

inline void queue_usbcode(uint32_t time, uint8_t flags, uint8_t keycode)
{
    // Trick - even if current buffer position is empty, write to the next one.
    // It may be empty because reader already processed it.
    USBQueue_writepos = KEYCODE_BUFFER_NEXT(USBQueue_writepos);
    // Make sure not to overwrite pending events - TODO think how not to fall into infinite loop if buffer full.
    while (USBQueue[USBQueue_writepos].keycode != USBCODE_NOEVENT)
    {
        USBQueue_writepos = KEYCODE_BUFFER_NEXT(USBQueue_writepos);
    }
    USBQueue[USBQueue_writepos].sysTime = time;
    USBQueue[USBQueue_writepos].flags = flags;
    USBQueue[USBQueue_writepos].keycode = keycode;
}

inline void play_macro(uint_fast16_t macro_start)
{
    uint8_t *mptr = &config.macros[macro_start] + 3;
    uint8_t *macro_end = mptr + config.macros[macro_start + 2];
    uint32_t now = systime;
    uint_fast16_t delay;
    uint8_t keyflags;
    while(mptr <= macro_end)
    {
        switch (*mptr >> 6)
        {
            case 0:
                // Press+release, timing from delayLib
                delay = config.delayLib[(*mptr >> 2) & 0x0f];
                mptr++;
                queue_usbcode(now, 0, *mptr);
                now += delay;
                queue_usbcode(now, USBQUEUE_RELEASED_MASK, *mptr);
                break;
            case 1:
                // Press or release
                keyflags = (*mptr & MACRO_KEY_UPDOWN_RELEASE) ? USBQUEUE_RELEASED_MASK : 0;
                mptr++;
                queue_usbcode(now, keyflags, *mptr);
                break;
            default: return;
        }
        mptr++;
    }
}

inline uint8_t process_scancode_buffer(void)
{
    if (scancode_buffer_readpos == scancode_buffer_writepos)
        return COMMONSENSE_NOKEY;
    // Skip zeroes that might be there
    while (scancode_buffer[scancode_buffer_readpos] == COMMONSENSE_NOKEY)
    {
        scancode_buffer_readpos = SCANCODE_BUFFER_NEXT(scancode_buffer_readpos);
    }
    uint8_t scancode = scancode_buffer[scancode_buffer_readpos];
#ifdef MATRIX_LEVELS_DEBUG
    xprintf("sc: %d %d @ %d ms, lvl %d/%d", scancode & KEY_UP_MASK, scancode & SCANCODE_MASK, systime, level_buffer[scancode_buffer_readpos], level_buffer_inst[scancode_buffer_readpos]);
#else
    //xprintf("sc: %d %d @ %d ms", scancode & KEY_UP_MASK, scancode &SCANCODE_MASK, systime);
#endif
    scancode_buffer[scancode_buffer_readpos] = COMMONSENSE_NOKEY;
    return scancode;
}

inline void push_back_scancode(uint8_t sc)
{
    scancode_buffer[scancode_buffer_readpos] = sc;
}

inline void process_real_key(void)
{
    uint8_t sc, usb_sc;
    // Real keys are processed there. So modifiers can be processed right away, not buffered.
    sc = process_scancode_buffer();
    if (sc == COMMONSENSE_NOKEY)
    {
            // Nothing to do.
            return;
    }
    if (sc == (COMMONSENSE_NOKEY | KEY_UP_MASK))
    {
/*
 * This is "All keys are up" signal, sun keyboard-style.
 * It is used to deal with stuck keys. Those appear due to layers - suppose you press the key, 
 * then switch layer which has another key at that SC position.
 * When you release the key - non-existent key release is generated, which is not that bad, but first key is stuck forever.
 */
        if (!USBQUEUE_IS_EMPTY)
        {
            push_back_scancode(sc);
        }
        else
        {
            reset_reports();
        }
        return;
    }
    if (status_register.setup_mode)
    {
        outbox.response_type = C2RESPONSE_SCANCODE;
        outbox.payload[0] = sc;
        usb_send_c2();
        return;
    }
    // Resolve USB keycode using current active layers
    for (uint8_t i=currentLayer; i >= 0; i--)
    {
        usb_sc = config.layers[i][sc & SCANCODE_MASK];
        if (usb_sc != USBCODE_TRANSPARENT)
        {
            break;
        }
    }
    //xprintf("SC->KC: %d -> %d", sc & SCANCODE_MASK, usb_sc);
    if (usb_sc < USBCODE_A)
    {
        // Dead key.
        return;
    }
    if ((usb_sc & 0xf8) == 0xa8)
    {
        process_layerMods(sc, usb_sc);
        return;
/*
TODO resolve problem where pressed mod keys are missing on the new layer.
 -> Do they get stuck?
 -> Do they automatically release?
*/
    }
    uint8_t keyflags = (sc & USBQUEUE_RELEASED_MASK) | USBQUEUE_REAL_KEY_MASK;
    uint_fast16_t macro_ptr = lookup_macro(keyflags, usb_sc);
    bool do_play = (macro_ptr != MACRO_NOT_FOUND);
    bool do_queue = !do_play; // eat the macro-producing code.
    if (do_play && (sc & USBQUEUE_RELEASED_MASK) && (config.macros[macro_ptr+1] & MACRO_TYPE_TAP))
    {
        // Tap macro. Check if previous event was this key down and it's not too late.
        do_queue = true;
        if ((usb_sc != pipeline_prev_usbkey) || (pipeline_prev_usbkey_time + config.delayLib[DELAYS_TAP] < systime))
        {
            // Nope!
            do_play = false;
        }
    }
    pipeline_prev_usbkey = usb_sc;
    pipeline_prev_usbkey_time = systime;
/*        
NOTE: queue_usbcode skips non-zero cells in the buffer. Think what to do on overflow.
NOTE2: we still want to maintain order?
    Otherwise linked list is probably what's doctor ordered (though expensive at 4B per pointer plus memory management)
*/
    if (do_queue) queue_usbcode(systime, keyflags, usb_sc);
    if (do_play) play_macro(macro_ptr);
    return;
}

#define NO_COOLDOWN USBQueue[pos].flags |= USBQUEUE_RELEASED_MASK;
/*
    Idea: not move readpos until keycode after it is processed. 
    So, readpos to writepos would be the working area.

    TODO: maintain bitmap of currently pressed keys to release them on reset and for better KRO handling.
    TODO: implement out of order key release?
    NOTE ^ ^^ very rare situations where this is needed.
 */ 
inline void update_reports(void)
{
    if (cooldown_timer > 0)
    {
        // Slow down! Delay 0 controls update rate. 
        // we should be called every millisecond - so setting delay0 to 10 will essentially makes it 100Hz keyboard 
        // with latency of 1kHz one.
        cooldown_timer--;
        return;
    }
    if (USBQUEUE_IS_EMPTY)
    {
        return;
    }
    // If there's change - find first non-empty buffer cell.
    // This results in infinite loop on empty buffer - must take care not to queue NOEVENTs.
    while (USBQueue[USBQueue_readpos].keycode == USBCODE_NOEVENT)
    {
        USBQueue_readpos = KEYCODE_BUFFER_NEXT(USBQueue_readpos);
    }
    //xprintf("USB queue %d - %d", USBQueue_readpos, USBQueue_writepos);
    uint8_t pos = USBQueue_readpos;
    do 
    {
        if (USBQueue[pos].keycode != USBCODE_NOEVENT && USBQueue[pos].sysTime <= systime)
        {
            if (USBQueue[pos].keycode < USBCODE_A)
            {
                // side effect - key transparent till the bottom will toggle exp. header
                // But it should not ever be put on queue!
                exp_toggle();
                NO_COOLDOWN
            }
            // Codes you want filtered from reports MUST BE ABOVE THIS LINE!
            // -> Think of special code for collectively settings mods!
            else if (USBQueue[pos].keycode >= 0xe8)
            {
                update_consumer_report(&USBQueue[pos]);
            }
            else if (USBQueue[pos].keycode >= 0xa5 && USBQueue[pos].keycode <= 0xa7)
            {
                update_system_report(&USBQueue[pos]);
            }
            else
            {
                update_keyboard_report(&USBQueue[pos]);
            }
            if ((USBQueue[pos].flags & USBQUEUE_RELEASED_MASK) == 0)
            {
                // We only throttle keypresses. Key release doesn't slow us down - 
                // minimum duration is guaranteed by fact that key release goes after key press and keypress triggers cooldown.
                cooldown_timer = config.delayLib[DELAYS_EVENT]; // Actual update happened - reset cooldown.
                exp_keypress(USBQueue[pos].keycode); // Let the downstream filter by keycode
            }
            USBQueue[pos].keycode = USBCODE_NOEVENT;
            if (pos == USBQueue_readpos && pos != USBQueue_writepos)
            {
                // Only if we're at first position. Previous cells may contain future actions otherwise!
                // Also if not the last item - we don't want to overrun the buffer.
                USBQueue_readpos = KEYCODE_BUFFER_NEXT(pos);
            }
            break;
        }
    } while (pos != USBQueue_writepos);
}

inline void pipeline_process(void)
{
    if (false)
    {
/*
        playing non-game macro
            if wait is blocking:
                if SCB has macro SC release:
                    set that SCB pos to 0
                    play after-wait part of macro to USBCB w/proper timestamps
*/
    }
    else
    {
        process_real_key();
    }
    update_reports();
}

inline bool pipeline_process_wakeup(void)
{
    uint8 sc = process_scancode_buffer();
    if (sc == COMMONSENSE_NOKEY)
    {
        return false;
    }
    if ((sc & KEY_UP_MASK) == 0)
    {
        return true;
    }
    return false;
}

void pipeline_init(void)
{
    // Pipeline is worked on in main loop only, no point disabling IRQs to avoid preemption.
    scan_reset();
    USBQueue_readpos  = 0;
    USBQueue_writepos = 0;
    cooldown_timer = 0;
    memset(USBQueue, USBCODE_NOEVENT, sizeof USBQueue);
    memset(pipeline_bitmap, 0, sizeof pipeline_bitmap);
}