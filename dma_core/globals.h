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
#define FORCE_BIT(VAR, BN, TO) ((VAR & (!(1<<BN))) + (TO << BN))
// /LIB.H!!!
#define BEAMSPRING 0
#define BUCKLING_SPRING 1

#define SWITCH_TYPE BEAMSPRING
#define MATRIX_COLS 24
#define MATRIX_ROWS 4

#include "c2/c2_protocol.h"
#include "c2/nvram.h"


// Main safety switch
#define NOT_A_KEYBOARD 1

#define DEVICE_VER_MAJOR      0x00
#define DEVICE_VER_MINOR      0x01

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
#define KRO_LIMIT 62

// USB stuff
#define KEYBOARD_EP 1
#define KBD_SCB USB_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_OUT_RPT_SCB
#define KBD_INBOX USB_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_OUT_BUF
#define KBD_OUTBOX USB_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_IN_BUF

#define CONSUMER_KRO_LIMIT 8
#define CONSUMER_EP 2
#define CONSUMER_OUTBOX USB_DEVICE0_CONFIGURATION0_INTERFACE2_ALTERNATE0_HID_IN_BUF

#define SYSTEM_EP 3
#define SYSTEM_OUTBOX USB_DEVICE0_CONFIGURATION0_INTERFACE3_ALTERNATE0_HID_IN_BUF

#define OUTBOX_EP 8
#define CTRLR_SCB USB_DEVICE0_CONFIGURATION0_INTERFACE1_ALTERNATE0_HID_OUT_RPT_SCB
#define CTRLR_INBOX USB_DEVICE0_CONFIGURATION0_INTERFACE1_ALTERNATE0_HID_OUT_BUF

IN_c2packet_t outbox;

// EEPROM stuff
psoc_eeprom_t config;

typedef struct {
    bool emergency_stop;
    bool matrix_output;
} __attribute__ ((packed)) status_register_t;

uint8_t tick;
uint32_t systime;
uint8_t usb_idletime;

status_register_t status_register;
uint8_t led_status;

void xprintf(const char *format_p, ...);

#if SWITCH_TYPE == BEAMSPRING
#define NORMALLY_LOW 0
#elif SWITCH_TYPE == BUCKLING_SPRING
#define NORMALLY_LOW 1
#else
#error "Unknown switch type"
#endif
