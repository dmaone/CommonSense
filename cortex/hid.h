/*
 *
 * Copyright (C) 2023 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * HID report conversion routines
 */

#pragma once
#include <stdint.h>

#include "pipeline.h"

#define KBD_KRO_LIMIT 62
#define CONSUMER_KRO_LIMIT 8
#define SYSTEM_KRO_LIMIT 1

/* Devices may append additional data to these boot reports,
 * but the first 8 bytes of keyboard reports
 * and the first 3 bytes of mouse reports
 * must conform to the format defined by the Boot Report descriptor
 * in order for the data to be correctly interpreted by the BIOS.
 * -- HID Spec, v1.11, Appendix B: "Boot Interface Descriptors"
 *
 * The BIOS will ignore any extensions to reports.
 * -- Same place.
 */
#define MAX_KEYS 126

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
} keyboard_state;
uint8_t keys_pressed;

// Consumer or system reports are not expected to be tested to KRO limit.
uint16_t consumer_state[CONSUMER_KRO_LIMIT];
uint8_t system_state[SYSTEM_KRO_LIMIT];

// state as seen by system. Need to avoid generation of reset messages when not
// needed, as well as for proper rollover state generation of keyboard report.
uint8_t keyboard_report[MAX_KEYS + 2];
uint16_t consumer_report[CONSUMER_KRO_LIMIT];
uint8_t system_report[SYSTEM_KRO_LIMIT];

void hid_init();
void hid_reset_reports();
void hid_update_keyboard(hid_event* key);
void hid_update_consumer(hid_event* key);
void hid_update_system(hid_event* key);
