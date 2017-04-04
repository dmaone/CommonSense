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

inline uint8_t process_scancode_buffer(void)
{
    if (scancode_buffer_readpos == scancode_buffer_writepos)
        return COMMONSENSE_NOKEY;
    // Skip zeroes that might be there
    while (scancode_buffer[scancode_buffer_readpos] == COMMONSENSE_NOKEY)
    {
        scancode_buffer_readpos = SCANCODE_BUFFER_NEXT(scancode_buffer_readpos);
CyPins_SetPin(ExpHdr_2);
CyDelayUs(1);
CyPins_ClearPin(ExpHdr_2);
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

inline void process_mods(uint8_t sc, uint8_t keycode)
{
    if ((sc & KEY_UP_MASK) == 0) {
        // Press
        mods |= (1 << (keycode & 0x07));
    } else {
        // Release
        mods &= ~(1 << (keycode & 0x07));
    }
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
// TODO need to generate COMMONSENSE_NOKEY release to signal no keys pressed - to deal with stuck keys.
// Those will appear due to layers - suppose you press the key, then switch layer which has another key at that SC position.
// When you release the key - non-existent key release is generated, which is not that bad, but first key is stuck forever.
// Need to do it only once after the last physical key is released.
// This also must clear USB queue of all physical keys pending. Special case "macro not playing" - just init all buffers.
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
        usb_sc = config.lmstash[i*(MATRIX_COLS*MATRIX_ROWS)+(sc & SCANCODE_MASK)];
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
    if ((usb_sc & 0xf8) == 0xe0)
    {
        process_mods(sc, usb_sc);
        queue_usbcode(systime, (sc & USBQUEUE_RELEASED) | USBQUEUE_REAL_KEY, 0xe0);
        return;
    }
/*
    Check for macro triggers here
        if USBC+mods = macro trigger:
            play macro to USBCB setting timestamps
        return;
*/

/*        
NOTE: queue_usbcode skips non-zero cells in the buffer. Think what to do on overflow.
NOTE2: we still want to maintain order?
    Otherwise linked list is probably what's doctor ordered (though expensive at 4B per pointer plus memory management)
*/
    queue_usbcode(systime, (sc & USBQUEUE_RELEASED) | USBQUEUE_REAL_KEY, usb_sc);
    return;
}

/*
    Idea: not move readpos until keycode after it is processed. 
    So, readpos to writepos would be the working area.
 */ 
inline void update_reports(void)
{
    if (USBQueue_readpos == USBQueue_writepos && USBQueue[USBQueue_readpos].keycode == USBCODE_NOEVENT)
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
            if (USBQueue[pos].keycode == 0xe0)
            {
                // Actual mods processed above and not pushed to keycode buffer.
                // Here it means "update report from bitmap we currently have".
                update_keyboard_mods(mods);
            }
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
    scan_reset();
    USBQueue_readpos  = 0;
    USBQueue_writepos = 0;
    memset(USBQueue, USBCODE_NOEVENT, sizeof(USBQueue));
}