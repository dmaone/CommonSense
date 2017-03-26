/*
 *
 * Copyright (C) 2016-2017 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
*/
#include "scan.h"

uint8_t process_scancode_buffer(void)
{
    if (scancode_buffer_readpos == scancode_buffer_writepos)
        return 0;
    scancode_buffer_readpos = SCANCODE_BUFFER_NEXT(scancode_buffer_readpos);
    // Skip zeroes that might be there
    // TODO infinite loop guard
    while (scancode_buffer[scancode_buffer_readpos] == 0)
    {
        scancode_buffer_readpos = SCANCODE_BUFFER_NEXT(scancode_buffer_readpos);
    }
    uint8_t scancode = scancode_buffer[scancode_buffer_readpos];
#ifdef MATRIX_LEVELS_DEBUG
    xprintf("sc: %d %d @ %d ms, lvl %d/%d", scancode & 0x80, scancode &0x7f, systime, level_buffer[scancode_buffer_readpos], level_buffer_inst[scancode_buffer_readpos]);
#else
    xprintf("sc: %d %d @ %d ms", scancode & 0x80, scancode &0x7f, systime);
#endif
    scancode_buffer[scancode_buffer_readpos] = 0;
    return scancode;
}

void do_pipeline(void)
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
        uint8_t sc = process_scancode_buffer();
/*
        pop SCB, SC
        SC->USBC // using current layer
        if USBC = layer change:
            do layer change
        |
TODO resolve problem where pressed mod keys are missing on the new layer.
 -> Do they get stuck?
 -> Do they automatically release?
        |
        else if USBC+mods = macro trigger:
            play macro to USBCB setting timestamps
        else:
            push USBCB, USBC@now
NOTE: "push" skips non-zero cells in the buffer. Think what to do on overflow.
NOTE2: we still want to maintain order?
    Otherwise linked list is probably what's doctor ordered (though expensive at 4B per pointer plus memory management)
                    
process_USB_scancode_buffer():
    find first non-zero older-than-now USBC
    update USB reports with that code
        -> Think of special code for collectively settings mods!
    zero sent USBC, advance read ptr
           
 */
    }
}

