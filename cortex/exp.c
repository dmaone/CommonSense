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

#ifdef INVERT_EXPHDR
#define EXPHDR_ON(X) CyPins_ClearPin(X)
#define EXPHDR_OFF(X) CyPins_SetPin(X)
#else
#define EXPHDR_ON(X) CyPins_SetPin(X)
#define EXPHDR_OFF(X) CyPins_ClearPin(X)
#endif

#ifdef INVERT_FIRE
#define FIRE_ON(X) CyPins_ClearPin(X)
#define FIRE_OFF(X) CyPins_SetPin(X)
#else
#define FIRE_ON(X) CyPins_SetPin(X)
#define FIRE_OFF(X) CyPins_ClearPin(X)
#endif

void exp_reset(void) {
  solenoid_on = false;
  solenoid_queue = 0;
  exp_cooldown = 0;
  EXPHDR_OFF(ExpHdr_0);
  EXPHDR_OFF(ExpHdr_1);
  FIRE_OFF(EXP_ACTUATION_PIN);
  EXPHDR_OFF(ExpHdr_3);
}

void exp_init(void) {
  mode = config.expMode;
  exp_reset();
  switch (mode) {
  case EXP_MODE_SOLENOID_NUMCAPS:
    EXPHDR_ON(EXP_SOLENOID_POWER);
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

#define SYNC_LED(MASK, PIN) (leds & MASK) ? EXPHDR_ON(PIN) : EXPHDR_OFF(PIN);

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
    FIRE_OFF(EXP_ACTUATION_PIN);
    solenoid_on = false;
  } else if (solenoid_queue > 0) {
    exp_cooldown = config.expParam1;
    FIRE_ON(EXP_ACTUATION_PIN);
    solenoid_on = true;
    solenoid_queue--;
  } else {
    exp_cooldown = 0;
  }
}

#undef EXPHDR_ON
#undef EXPHDR_OFF
#undef FIRE_ON
#undef FIRE_OFF
