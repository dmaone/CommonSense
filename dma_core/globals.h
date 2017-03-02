/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
*/

#pragma once
#include "c2/c2_protocol.h"
#include "c2/nvram.h"

// Main safety switch
#define NOT_A_KEYBOARD 1

#define DEVICE_VER_MAJOR      0x00
#define DEVICE_VER_MINOR      0x01

#define KRO_LIMIT 62

// USB stuff
#define KEYBOARD_EP 1
#define OUTBOX_EP 8

#define INBOX_EP 7
#define USB_EP_7_ISR_EXIT_CALLBACK
// ^ ^^ those must be kept in sync!

//for using ## in macro definition. Unfortunately doesn't work in macro names :(
#define FUZE_3_TOKENS(x, y, z) x ## y ## z
#define JOIN3(x, y, z) FUZE_3_TOKENS(x, y, z)

#define INBOX_CALLBACK JOIN3(USB_EP_, INBOX_EP, _ISR_ExitCallback)

void INBOX_CALLBACK(void);

bool message_for_you_in_the_lobby;
OUT_c2packet_t inbox;
IN_c2packet_t outbox;

// EEPROM stuff
psoc_eeprom_t config;

typedef struct {
    bool emergency_stop;
    bool matrix_output;
} __attribute__ ((packed)) status_register_t;

status_register_t status_register;

void xprintf(const char *format_p, ...);