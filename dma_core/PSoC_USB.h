/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
*/

#pragma once
#include "scan.h"
#include "pipeline.h"

#define SUSPEND_SYSTIMER_DIVISOR 10

enum devicePowerStates {
    DEVSTATE_FULL_THROTTLE = 0,
    DEVSTATE_SLEEP,
    DEVSTATE_WATCH,
    DEVSTATE_SUSPENDING,
    DEVSTATE_RESUMING,
};

uint8_t power_state;

void usb_init(void);
void usb_configure(void);

void usb_send_c2();
void usb_wakeup(void);
void process_msg(OUT_c2packet_t *);
void load_config(void);

void update_keyboard_report(queuedScancode *key);
void update_keyboard_mods(uint8_t);
void update_consumer_report(queuedScancode *key);
void update_system_report(queuedScancode *key);

// Wait for EP - stop waiting if suspend looms.
#if NOT_A_KEYBOARD == 1
#define _WIPE_OUTBOX(OUTBOX) memset(OUTBOX, 0, OUTBOX_SIZE(OUTBOX))
#else
#define _WIPE_OUTBOX(OUTBOX)
#endif

#define USB_WAIT_FOR_EP(EP) while (USB_GetEPState(EP) != USB_IN_BUFFER_EMPTY) { if (power_state != DEVSTATE_FULL_THROTTLE) return; }

#define USB_SEND_REPORT(TYPE) USB_WAIT_FOR_EP(TYPE##_EP); _WIPE_OUTBOX(TYPE##_OUTBOX); USB_LoadInEP(TYPE##_EP, TYPE##_OUTBOX, OUTBOX_SIZE(TYPE##_OUTBOX));
