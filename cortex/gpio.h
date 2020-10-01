/*
 *
 * Copyright (C) 2017 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#pragma once
#include <stdint.h>

#define LED_NUMLOCK_MASK 0x01
#define LED_CAPSLOCK_MASK 0x02
#define LED_SCRLOCK_MASK 0x04

#define EXP_CAPSLOCK_PIN ExpHdr_3
#define EXP_NUMLOCK_PIN ExpHdr_1
#define EXP_SCRLOCK_PIN ExpHdr_0

// Comes on briefly when a key is pressed
#define EXP_ACTUATION_PIN ExpHdr_2

// Provides solenoid enable signal, for xwhatsit solenoid board compatibility.
#define EXP_SOLENOID_POWER ExpHdr_0

void gpio_init(void);
void gpio_toggle_expHdr(void);
void gpio_tick(uint8_t tick);
void gpio_setLEDs(uint8_t status);
void gpio_keypress(uint8_t keycode);
