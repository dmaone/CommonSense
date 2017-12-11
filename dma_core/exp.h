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

#define EXP_NUMLOCK_PIN ExpHdr_0
#define EXP_CAPSLOCK_PIN ExpHdr_1
#define EXP_SCRLOCK_PIN ExpHdr_2

#define EXP_SOLENOID_POWER ExpHdr_0
#define EXP_SOLENOID_CONTROL ExpHdr_2

void exp_init(void);
void exp_toggle(void);
void exp_tick(uint8_t tick);
void exp_setLEDs(uint8_t status);
void exp_keypress(uint8_t keycode);