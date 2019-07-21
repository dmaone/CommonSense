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

// #define DEBUG_STATE_MACHINE

// #define DEBUG_PIPELINE

// ExpHdr0 used to signal interrupt start and end
// #define DEBUG_INTERRUPTS

// ExpHdr0 is set for 1us for keypress, 2us for release
// #define DEBUG_SHOW_KEYPRESSES

#define BEAMSPRING 0
#define BUCKLING_SPRING 1
#define ADB 2
#define SUN 3
#define CORTRON 4

#define SCANNER_CS 0
#define SCANNER_ADB 1
#define SCANNER_SUN 2
#define SCANNER_CORTRON 3
// Do not touch above definitions. To change switch type change config.h

#include "config.h"

// Main safety switch
#define NOT_A_KEYBOARD 0

// LIB.H!!!
#define TEST_BIT(VAR, BN) (VAR & (1 << (BN)))
//#define CLEAR_BIT(VAR, BN) {xprintf("CLR %d %d", VAR, BN); VAR &= !(1 << BN); }
//#define SET_BIT(VAR, BN) {xprintf("SET %d %d", VAR, BN); VAR |= (1 << BN); }
#define CLEAR_BIT(VAR, BN) VAR &= ~(1 << (BN))
#define SET_BIT(VAR, BN) VAR |= (1 << (BN))
#define FLIP_BIT(VAR, BN) VAR ^= (1 << (BN))
#define FORCE_BIT(VAR, BN, TO) { CLEAR_BIT(VAR, BN); if (TO) SET_BIT(VAR, BN); }
// /LIB.H!!!

#include "../c2/c2_protocol.h"
#include "../c2/nvram.h"

#define DEVICE_VER_MAJOR 0x01
#define DEVICE_VER_MINOR 0x00

/* Devices may append additional data to these boot reports,
 * but the first 8 bytes of keyboard reports
 * and the first 3 bytes of mouse reports
 * must conform to the format defined by the Boot Report descriptor
 * in order for the data to be correctly interpreted by the BIOS.
 * -- HID Spec, v1.11, Appendix B: "Boot Interface Descriptors"
 *
 * The BIOS will ignore any extensions to reports.
 * -- Same place.
 */
#define MAX_KEYS 126

// USB stuff
#define USB_REMOTE_WAKEUP

#define KBD_KRO_LIMIT 62
#define KBD_EP 1
#define KBD_SCB USB_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_OUT_RPT_SCB
#define KBD_INBOX USB_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_OUT_BUF
#define KBD_OUTBOX USB_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_IN_BUF

#define CONSUMER_KRO_LIMIT 8
#define CONSUMER_EP 2
#define CONSUMER_OUTBOX                                                        \
  USB_DEVICE0_CONFIGURATION0_INTERFACE2_ALTERNATE0_HID_IN_BUF

#define SYSTEM_EP 3
#define SYSTEM_OUTBOX                                                          \
  USB_DEVICE0_CONFIGURATION0_INTERFACE3_ALTERNATE0_HID_IN_BUF

#define OUTBOX_EP 8
#define CTRLR_SCB                                                              \
  USB_DEVICE0_CONFIGURATION0_INTERFACE1_ALTERNATE0_HID_OUT_RPT_SCB
#define CTRLR_INBOX USB_DEVICE0_CONFIGURATION0_INTERFACE1_ALTERNATE0_HID_OUT_BUF

#define OUTBOX_SIZE(X) (sizeof(X) - 1)

IN_c2packet_t outbox;

// EEPROM stuff
psoc_eeprom_t config;

#define PIN_DEBUG(POSITION, DELAY)                                             \
  CyPins_SetPin(ExpHdr_##POSITION);                                            \
  CyDelayUs(DELAY);                                                            \
  CyPins_ClearPin(ExpHdr_##POSITION);

// Modified by ISR!
volatile uint8_t tick;
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

uint8_t output_direction;
enum outputDirection {
  OUTPUT_DIRECTION_NONE = 0,
  OUTPUT_DIRECTION_USB,
  OUTPUT_DIRECTION_SERIAL,
  
  OUTPUT_DIRECTION_MAX
};

void xprintf(const char *format_p, ...);

#if SWITCH_TYPE == BEAMSPRING
#define SCANNER_TYPE SCANNER_CS
#define NORMALLY_LOW 0
#elif SWITCH_TYPE == BUCKLING_SPRING
#define SCANNER_TYPE SCANNER_CS
#define NORMALLY_LOW 1
#elif SWITCH_TYPE == ADB
#define SCANNER_TYPE SCANNER_ADB
#define NORMALLY_LOW 0
#elif SWITCH_TYPE == SUN
#define SCANNER_TYPE SCANNER_SUN
#define NORMALLY_LOW 0
#elif SWITCH_TYPE == CORTRON
#define SCANNER_TYPE CORTRON
#define NORMALLY_LOW 1
#else
#error "Unknown switch type"
#endif
