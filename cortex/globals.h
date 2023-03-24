/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/*
 * NO USER-SERVICEABLE PARTS INSIDE. You want config.h!
 */
#pragma once
#if __has_include("cytypes.h")
#include "cytypes.h"
#endif

// xprintf is no-op outside of setup mode. This switch overrides that.
// #define XPRINTF_ALWAYS_ENABLED

// same, for coded_[x]_message()
// #define CODED_MESSAGES_ALWAYS_ENABLED

// WARNING consider uncommenting ^ when enabling DEBUG_<anything>

// #define DEBUG_STATE_MACHINE

// #define DEBUG_PIPELINE

// ExpHdr0 used to signal interrupt start and end
// #define DEBUG_INTERRUPTS

// ExpHdr0 is set for 1us for keypress, 2us for release
// #define DEBUG_SHOW_KEYPRESSES

// MUST BE KEPT IN SYNC WITH c2_protocol.h for names!
#define BEAMSPRING 0
#define BUCKLING_SPRING 1
#define ADB 2
#define SUN 3
#define CORTRON 4
#define MICROSWITCH 5
#define CONTACT 6
// Do not touch above definitions. To change switch type change config.h

#include "config.h"

// Main safety switch
#define NOT_A_KEYBOARD 0

// Should matrix generate "All keys up" signal when no keys are pressed?
#undef MATRIX_GENERATES_ALL_UP

// If FlightController stops polling us - return to normal mode after X ticks.
#define SETUP_TIMEOUT 2000
#define LARGE_ENOUGH_32 (1 << 30);

// LIB.H!!!
#define TEST_BIT(VAR, BN) (VAR & (1 << (BN)))
#define BIT_IS_SET(VAR, BN) (VAR & (1 << (BN)))
#define BIT_IS_CLEAR(VAR, BN) ((VAR & (1 << (BN))) == 0)
//#define CLEAR_BIT(VAR, BN) {xprintf("CLR %d %d", VAR, BN); VAR &= !(1 << BN); }
//#define SET_BIT(VAR, BN) {xprintf("SET %d %d", VAR, BN); VAR |= (1 << BN); }
#define CLEAR_BIT(VAR, BN) VAR &= ~(1 << (BN))
#define SET_BIT(VAR, BN) VAR |= (1 << (BN))
#define FLIP_BIT(VAR, BN) VAR ^= (1 << (BN))
#define FORCE_BIT(VAR, BN, TO) { CLEAR_BIT(VAR, BN); if (TO) SET_BIT(VAR, BN); }
// /LIB.H!!!

#include "../c2/c2_protocol.h"
#include "../c2/nvram.h"

#define DEVICE_VER_MAJOR 0x02
#define DEVICE_VER_MINOR 0x00

// USB stuff
#define USB_REMOTE_WAKEUP

#define KBD_EP 1
#define CONSUMER_EP 2
#define SYSTEM_EP 3
#define OUTBOX_EP 8

// #else
// #define SYSTEM_OUTBOX 15
// #endif
#define OUTBOX_SIZE(X) (sizeof(X) - 1)

// EEPROM stuff
psoc_eeprom_t config;

#define PIN_DEBUG(POSITION, DELAY)                                             \
  CyPins_SetPin(ExpHdr_##POSITION);                                            \
  CyDelayUs(DELAY);                                                            \
  CyPins_ClearPin(ExpHdr_##POSITION);

// Modified by ISR!
volatile uint32_t systime;

enum devicePowerStates {
  DEVSTATE_FULL_THROTTLE = 1, // Going full bore
  DEVSTATE_PREPARING_TO_SLEEP, // USB suspend condition detected
  DEVSTATE_SLEEP, // USB suspend, not allowed to bring host from standby
  DEVSTATE_WATCH, // USB suspend, monitoring for remote wakeup
  DEVSTATE_SUSPENDING, // USB suspend condition detected
  DEVSTATE_RESUMING, // Host-initiated USB wakeup
  DEVSTATE_SLEEP_REQUEST, // external controller wants us to sleep.
  DEVSTATE_MAX
};
// Modified by ISR!
volatile uint8_t power_state;

uint16_t sanity_check_timer;
uint8_t status_register;
uint8_t led_status;
#define STATUS_IS(X) (TEST_BIT(status_register, X) != 0)
#define STATUS_NOT(X) (TEST_BIT(status_register, X) == 0)

uint8_t output_direction;
enum outputDirection {
  OUTPUT_DIRECTION_NONE = 0,
  OUTPUT_DIRECTION_USB,
  OUTPUT_DIRECTION_SERIAL,

  OUTPUT_DIRECTION_MAX
};

void xprintf(const char *format_p, ...);
void ts_xprintf(const char *format_p, ...);

#if SWITCH_TYPE == BEAMSPRING
#define NORMALLY_LOW 0
#elif SWITCH_TYPE == BUCKLING_SPRING
#define NORMALLY_LOW 1
#elif SWITCH_TYPE == ADB
// Nothing, really
#elif SWITCH_TYPE == SUN
// Nothing, really
#elif SWITCH_TYPE == CORTRON
#define MIN_CHARGE_DELAY 20
#define MIN_DISCHARGE_DELAY 4
#elif SWITCH_TYPE == MICROSWITCH
// Nothing, really
#elif SWITCH_TYPE == CONTACT
// Nothing, really
#else
#error "Unknown switch type"
#endif

// Defaults
#ifndef NORMALLY_LOW
#define NORMALLY_LOW 1
#endif
#ifndef MIN_CHARGE_DELAY
#define MIN_CHARGE_DELAY 2
#endif

#ifndef MIN_DISCHARGE_DELAY
#define MIN_DISCHARGE_DELAY 2
#endif
