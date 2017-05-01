/*
 *
 * Copyright (C) 2017 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
*/

#include <project.h>

#include "exp.h"
#include "../c2/c2_protocol.h"

static uint8_t leds;
static uint8_t solenoid_queue;
static bool solenoid_on;
static uint8_t exp_cooldown;
static uint8_t mode;

void exp_reset(void)
{
    solenoid_queue = 0;
    solenoid_on = 0;
    exp_cooldown = 0;
    CyPins_ClearPin(ExpHdr_0);
    CyPins_ClearPin(ExpHdr_1);
    CyPins_ClearPin(ExpHdr_2);
    CyPins_ClearPin(ExpHdr_3);
}

void exp_init(void)
{
    mode = config.expMode;
    exp_reset();
    switch(mode) {
        case EXP_MODE_SOLENOID:
            CyPins_SetPin(EXP_SOLENOID_POWER);
            break;
        case EXP_MODE_LEDS:
            break;
        default:
            break;
    }
}

void exp_toggle(void)
{
    if (mode == EXP_MODE_DISABLED)
    {
        exp_init();
    }
    else
    {
        mode = EXP_MODE_DISABLED;
        exp_reset();
    }
}

#define EXP_LED(MASK, PIN) if (leds & MASK) CyPins_SetPin(PIN); else CyPins_ClearPin(PIN);
void exp_setLEDs(uint8_t status)
{
    leds = status;
    switch (mode) {
        case EXP_MODE_LEDS:
            EXP_LED(LED_NUMLOCK_MASK, EXP_NUMLOCK_PIN);
            EXP_LED(LED_CAPSLOCK_MASK, EXP_CAPSLOCK_PIN);
            EXP_LED(LED_SCRLOCK_MASK, EXP_SCRLOCK_PIN);
            break;
        default:
            break;
    }
}

void exp_keypress(__attribute__ ((unused)) uint8_t keycode)
{
    switch (mode) {
        case EXP_MODE_SOLENOID:
            solenoid_queue++;
            break;
        default:
            break;
    }
}

void exp_tick(uint8_t tick)
{
    switch (mode) {
        case EXP_MODE_SOLENOID:
            if (exp_cooldown > tick)
            {
                exp_cooldown -= tick;
                return;
            }
            if (solenoid_on)
            {
                exp_cooldown = config.expParam2;
                CyPins_ClearPin(EXP_SOLENOID_CONTROL);
                solenoid_on = false;
            }
            else if (solenoid_queue > 0)
            {
                exp_cooldown = config.expParam1;
                CyPins_SetPin(EXP_SOLENOID_CONTROL);
                solenoid_on = true;
                solenoid_queue--;
            }
            else
            {
                exp_cooldown = 0;
            }
            break;
        default:
            break;
    }
}