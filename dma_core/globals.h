/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
*/

/*
Proto kit connectors.
      WO     O    WB     B   WBr    Br
J1  P2.0  P2.2  P0.7  P0.6  P2.4  P2.5
J2 P15.1 P15.0  P1.2  P1.3  P1.4  P1.5
J3  P3.6  P3.5  P3.4  P3.3  P3.1  P3.0
J4  P2.6  P2.7  P0.5  P0.1  P0.0 P15.5

F122: cols: 4-3-2(O+B), rows 2(Br)-1
*/

#pragma once
#include "c2/c2_protocol.h"
#include "c2/nvram.h"
#include "config.h"

#define KRO_LIMIT 62
// USB stuff
#define OUTBOX_EP 8
#define KEYBOARD_EP 1

//for using ## in macro definition. Unfortunately doesn't work in macro names :(
#define FUZE_3_TOKENS(x, y, z) x ## y ## z
#define JOIN3(x, y, z) FUZE_3_TOKENS(x, y, z)

#define INBOX_EP 7
#define USB_EP_7_ISR_EXIT_CALLBACK
// ^ ^^ those must be kept in sync!

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