/*
 *
 * Copyright (C) 2017 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "globals.h"
#include <project.h>

#include "exp.h"

static uint8_t mode;

static uint8_t leds;

static bool solenoid_on;
static uint8_t solenoid_queue;
static uint8_t exp_cooldown;

void exp_reset(void) {
  solenoid_on = false;
  solenoid_queue = 0;
  exp_cooldown = 0;
  CyPins_ClearPin(ExpHdr_0);
  CyPins_ClearPin(ExpHdr_1);
  CyPins_ClearPin(ExpHdr_2);
  CyPins_ClearPin(ExpHdr_3);
}

void exp_init(void) {
  mode = config.expMode;
  exp_reset();
  switch (mode) {
  case EXP_MODE_SOLENOID_NUMCAPS:
    CyPins_SetPin(EXP_SOLENOID_POWER);
    break;
  case EXP_MODE_LEDS:
    break;
  default:
    break;
  }
}

void exp_toggle(void) {
  if (mode == EXP_MODE_DISABLED) {
    exp_init();
  } else {
    mode = EXP_MODE_DISABLED;
    exp_reset();
  }
}

#define SYNC_LED(MASK, PIN) \
  if (leds & MASK)         \
    CyPins_SetPin(PIN);    \
  else                     \
    CyPins_ClearPin(PIN);

void exp_setLEDs(uint8_t status) {
  leds = status;
  SYNC_LED(LED_NUMLOCK_MASK, EXP_NUMLOCK_PIN);
  SYNC_LED(LED_CAPSLOCK_MASK, EXP_CAPSLOCK_PIN);
  if (mode == EXP_MODE_LEDS) {
    SYNC_LED(LED_SCRLOCK_MASK, EXP_SCRLOCK_PIN);
  }
}

void exp_keypress(__attribute__((unused)) uint8_t keycode) {
  solenoid_queue++;
}

void exp_tick(uint8_t tick) {
  if (mode == EXP_MODE_DISABLED) {
    return;
  }

  if (exp_cooldown > tick) {
    exp_cooldown -= tick;
    return;
  }
  if (solenoid_on) {
    exp_cooldown = config.expParam2;
    CyPins_ClearPin(EXP_ACTUATION_PIN);
    solenoid_on = false;
  } else if (solenoid_queue > 0) {
    exp_cooldown = config.expParam1;
    CyPins_SetPin(EXP_ACTUATION_PIN);
    solenoid_on = true;
    solenoid_queue--;
  } else {
    exp_cooldown = 0;
  }
}
