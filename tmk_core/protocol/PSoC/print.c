/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
*/

//#define print(s) xputs(s)
//#define println(s) xputs(s "\r\n")
#include <stdio.h>
#include <stdarg.h>
#include "usb_driver.h"
#include "globals.h"

// WARNING This function takes about 2K of flash. First candidate to footprint optimization.
void xprintf(const char *format_p, ...)
{
    va_list va;
    va_start(va, format_p);
    vsnprintf((char *)outbox.raw, 64, format_p, va);
    va_end(va);
    usb_send();
} 
