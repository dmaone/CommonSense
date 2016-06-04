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
#include "config.h"

uint8_t keyboard_leds(void);

// USB stuff
#define INBOX_EP 8
#define OUTBOX_EP 2
bool message_for_you_in_the_lobby;
OUT_c2packet_t inbox;
IN_c2packet_t outbox;

// EEPROM stuff
psoc_eeprom_t config;

//debugging
typedef struct {
     bool matrix_output;
} __attribute__ ((packed)) ephemeral_debug_t;

ephemeral_debug_t ephemeral_debug;
