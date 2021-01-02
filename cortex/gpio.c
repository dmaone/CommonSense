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

#include "gpio.h"
#include "scan.h"

static uint8_t mode;

static uint8_t leds;

static bool solenoid_on;
static uint8_t solenoid_queue;
static uint8_t exp_cooldown;

static int8_t gpio_press_threshold = -2;
static int8_t gpio_release_threshold = 2;

#if NUM_PEDALS > 0
static int8_t gpio_debounce_buffer[NUM_PEDALS];
#endif

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

void gpio_reset(void) {
  solenoid_on = false;
  solenoid_queue = 0;
  exp_cooldown = 0;
  EXPHDR_OFF(ExpHdr_0);
  EXPHDR_OFF(ExpHdr_1);
  FIRE_OFF(EXP_ACTUATION_PIN);
  EXPHDR_OFF(ExpHdr_3);
}

void gpio_init(void) {
  mode = config.expMode;
  gpio_reset();
  switch (mode) {
  case EXP_MODE_SOLENOID_NUMCAPS:
    EXPHDR_ON(EXP_SOLENOID_POWER);
    break;
  case EXP_MODE_LEDS:
    break;
  default:
    break;
  }
  // see scan_common_init() in scan_common.c for explanations
  gpio_release_threshold = config.pedalDebouncingTicks + 1;
  gpio_press_threshold = 0 - gpio_release_threshold;

#if NUM_PEDALS > 0
  for (uint8_t i = 0; i < NUM_PEDALS; i++) {
    gpio_debounce_buffer[i] = -1;
  }
#endif

}

void gpio_toggle_expHdr(void) {
  if (mode == EXP_MODE_DISABLED) {
    gpio_init();
  } else {
    mode = EXP_MODE_DISABLED;
    gpio_reset();
  }
}

#define SYNC_LED(MASK, PIN) (leds & MASK) ? EXPHDR_ON(PIN) : EXPHDR_OFF(PIN);

void gpio_setLEDs(uint8_t status) {
  leds = status;
  SYNC_LED(LED_NUMLOCK_MASK, EXP_NUMLOCK_PIN);
  SYNC_LED(LED_CAPSLOCK_MASK, EXP_CAPSLOCK_PIN);
  if (mode == EXP_MODE_LEDS) {
    SYNC_LED(LED_SCRLOCK_MASK, EXP_SCRLOCK_PIN);
  }
}

void gpio_keypress(__attribute__((unused)) uint8_t keycode) {
  solenoid_queue++;
}

static inline void process_solenoid(uint8_t usb_ticks_passed) {
  if (mode == EXP_MODE_DISABLED) {
    return;
  }

  if (exp_cooldown > usb_ticks_passed) {
    exp_cooldown -= usb_ticks_passed;
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

#if NUM_PEDALS == 0

static inline void process_pedals() {}

#else

// static uint8_t pedal_state[NUM_PEDALS];

static inline void register_event(uint8_t flags, uint8_t pedal) {
  if (config.pedal_flags[pedal] & PEDAL_EDGE_TRIGGERED) {
    // trigger a press-release
    scan_register_event(0, COMMONSENSE_MATRIX_SIZE + pedal);
    scan_register_event(KEY_UP_MASK, COMMONSENSE_MATRIX_SIZE + pedal);
    return;
  }
  scan_register_event(flags, COMMONSENSE_MATRIX_SIZE + pedal);
}

static inline void debounce_pedal(uint8_t isPressed, uint8_t pedal) {
  int8_t d_buf = gpio_debounce_buffer[pedal];
  bool keyUp = config.pedal_flags[pedal] & PEDAL_NORMALLY_HIGH ? isPressed > 0
                                                               : isPressed == 0;
  // See scan_common.c for details - this is exact copypasta.
  if (keyUp) {
    if (d_buf <= 0) { // Still released. Reset the streak counter.
      d_buf = -1;
    } else { // Was pressed. Bump the streak counter.
      if (++d_buf >= gpio_release_threshold) { // Release the hounds!
        d_buf = -1;
        register_event(KEY_UP_MASK, pedal);
      }
    }
  } else if (d_buf > 0) { // Pressed, no change. Reset the streak counter.
    d_buf = 1;
  } else { // Known pressed, was released. Bump the streak counter.
    if (--d_buf <= gpio_press_threshold) { // Long enough. Press the key.
      d_buf = 1;
        register_event(0, pedal);
    }
  }
  gpio_debounce_buffer[pedal] = d_buf;
}

static inline void process_pedals() {
  uint8_t readout = FSW0_Read();
  for (uint8_t i = 0; i < NUM_PEDALS; ++i) {
    debounce_pedal((readout & (1 << (7 - i))), i);
  }
}
#endif


void gpio_tick(uint8_t usb_ticks_passed) {
  process_solenoid(usb_ticks_passed);
  process_pedals();
}

#undef EXPHDR_ON
#undef EXPHDR_OFF
#undef FIRE_ON
#undef FIRE_OFF
