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
//#include <stdarg.h>
#include "usb_driver.h"
//#include "config.h" // for DEBUG_EP
#include "globals.h"

void xprintf(const char *format_p, ...)
{
//    va_list va;
//    va_start(va, format_p);
//    vsnprintf((char *)outbox, 64, format_p, va);
//    va_end(va);
    memcpy(outbox.raw, format_p, sizeof(outbox));
    usb_send();
} 
/*
void xsprintf(char *dest, const char format_p, ...)
{
    
    va_list va;
    va_start(va, format_p);
}
*/
/*
void xfprintf(void(*func)(uint8_t) *result, const char *format_p, ...)
{
    va_list va;
    va_start(va, format_p);
}
*/