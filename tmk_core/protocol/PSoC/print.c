#define print(s) xputs(s)
#define println(s) xputs(s "\r\n")
#include <stdio.h>
#include <stdarg.h>
#include "usb_driver.h"
#include "config.h" // for DEBUG_EP

void xprintf(const char *format_p, ...)
{
    uint8 buf[64];
    va_list va;
    va_start(va, format_p);
    memset(buf, 0x42, sizeof(buf));
    uint16 len = vsnprintf((char *)buf, 64, format_p, va);
    buf[len+1] = 0x41;
    va_end(va);
//    uint8 cur_state = USB_GetEPState(DEBUG_EP);
//    while (USB_GetEPState(DEBUG_EP) == USB_NO_EVENT_ALLOWED) {
//    }
    USB_LoadInEP(DEBUG_EP, buf, 64);
    /* Wait for ACK after loading data. */
    while (0u == USB_GetEPAckState(DEBUG_EP))
    {
    }
//    CyDelay(500);
    //send_debug(buf, len);
} 

void xsprintf(char *dest, const char format_p, ...)
{
    
    va_list va;
    va_start(va, format_p);
}

/*
void xfprintf(void(*func)(uint8_t) *result, const char *format_p, ...)
{
    va_list va;
    va_start(va, format_p);
}
*/