/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#pragma once
#include "pipeline.h"

#define SUSPEND_SYSTIMER_DIVISOR 10


/*
 * Internal state storage. Must be longer than conceivable number of
 * simultaneously pressed keys. USB HID spec requires whole report to say
 * "Keyboard Rollover Error" when it happens. OS (Windows at least) will
 * maintain last known state before KRO. Currently there's a semi-bug - if you
 * set KRO lower than OUTBOX_SIZE, you'll see keycodes beyond KRO limit. It's a
 * semi-bug because you're supposed to set KRO limit to match your packet size -
 * hence match OUTBOX_SIZE.
 */
union {
  struct {
    uint8_t mods;
    uint8_t reserved;
    uint8_t keys[MAX_KEYS];
  } __attribute__((packed));
  uint8_t raw[MAX_KEYS + 2];
} keyboard_report;
uint8_t keys_pressed;

// Consumer or system reports are not expected to be tested to KRO limit.
uint16_t consumer_report[CONSUMER_KRO_LIMIT];
uint8_t system_report[OUTBOX_SIZE(SYSTEM_OUTBOX)];

volatile int32_t ticksToAutonomy;

void usb_init(void);
void usb_configure(void);
void usb_tick(void);
void usb_nap(void);
void usb_check_power(void);
void usb_wake(void);

void usb_send_c2();
void usb_send_c2_blocking();
void usb_send_wakeup(void);
void usb_receive(OUT_c2packet_t *);
void load_config(void);
void apply_config(void);

void reset_reports();
void update_keyboard_report(queuedScancode *key);
void update_consumer_report(queuedScancode *key);
void update_system_report(queuedScancode *key);

#if NOT_A_KEYBOARD == 1
#define _WIPE_OUTBOX(OUTBOX) memset(OUTBOX, 0, OUTBOX_SIZE(OUTBOX))
#else
#define _WIPE_OUTBOX(OUTBOX)
#endif

#define USB_SEND_REPORT(TYPE)                                                  \
  _WIPE_OUTBOX(TYPE##_OUTBOX);                                                 \
  usbEnqueue(TYPE##_EP, OUTBOX_SIZE(TYPE##_OUTBOX), TYPE##_OUTBOX);
