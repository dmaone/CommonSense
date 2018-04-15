/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#pragma once
// LIB.H!!!
#define TEST_BIT(VAR, BN) (VAR & (1 << BN))
//#define CLEAR_BIT(VAR, BN) {xprintf("CLR %d %d", VAR, BN); VAR &= !(1 << BN); }
//#define SET_BIT(VAR, BN) {xprintf("SET %d %d", VAR, BN); VAR |= (1 << BN); }
#define CLEAR_BIT(VAR, BN) VAR &= ~(1 << BN)
#define SET_BIT(VAR, BN) VAR |= (1 << BN)
#define FORCE_BIT(VAR, BN, TO) { CLEAR_BIT(VAR, BN); if (TO) SET_BIT(VAR, BN); }
#define BEAMSPRING 0
#define BUCKLING_SPRING 1
// /LIB.H!!!

// Do not touch above definitions. To change switch type change this.
#define SWITCH_TYPE BUCKLING_SPRING

// Main safety switch
#define NOT_A_KEYBOARD 0

/*
 * if SELF_POWERED is defined - the device is self/battery-powered.
 * What does this mean:
 * \USB:VBUS\ pin MUST be assigned to one of the SIO pins (P12[0]-P12[7])
 * That pin must be connected to a point which is connected to VBUS
 * but deenergized when USB is disconnected.
 * NOTE: having a diode between device power rail and USB is not enough.
 * You must add a resistor between VBUS and the ground on USB side.
 * I experimented with 10kOhm - but that's 0.5mA which will make achieving
 * USB standby compliance pretty hard.
 * I think 100kOhm will work just fine - although you _may_ have to
 * increase POWER_CHECK_DELAY in PSoC_USB.c
 *
 * Also, if your power source is <3.6V (No, Li-ion/LiPoly are not <3.6V!)
 * you MUST set USB_POWER_MODE to USB_3V_OPERATION.
 * If you don't - USB will likely not work.
 * If you do and power from 5V though - you'll most likely fry USB regulator.
 */
#define SELF_POWERED
#define USB_POWER_MODE USB_5V_OPERATION

/*
 * pin assignment: direct order, aligned to the right of ADC.
 * for dual ADCs it means column X..11, X+12..23
 *
 * ODD NUMBER OF COLUMNS DO NOT MIX WITH DUAL ADCs!!! See scan.c!
 */
#define MATRIX_COLS 12
#define MATRIX_ROWS 8
#define MATRIX_LAYERS 4

#include "../c2/c2_protocol.h"
#include "../c2/nvram.h"

#undef DEBUG_STATE_MACHINE

// ExpHdr0 used to signal interrupt start and end
#undef DEBUG_INTERRUPTS

// ExpHdr0 is set for 1us for keypress, 2us for release
#undef DEBUG_SHOW_KEYPRESSES

#define DEVICE_VER_MAJOR 0x00
#define DEVICE_VER_MINOR 0x01

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
  DEVSTATE_FULL_THROTTLE = 1,
  DEVSTATE_PREPARING_TO_SLEEP,
  DEVSTATE_SLEEP,
  DEVSTATE_WATCH,
  DEVSTATE_SUSPENDING,
  DEVSTATE_RESUMING,
  DEVSTATE_SHUTDOWN_REQUEST,
  DEVSTATE_MAX
};
// Modified by ISR!
volatile uint8_t power_state;

uint16_t sanity_check_timer;
uint8_t status_register;
uint8_t led_status;

void xprintf(const char *format_p, ...);

#if SWITCH_TYPE == BEAMSPRING
#define NORMALLY_LOW 0
#elif SWITCH_TYPE == BUCKLING_SPRING
#define NORMALLY_LOW 1
#else
#error "Unknown switch type"
#endif
