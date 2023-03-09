/*
 *
 * Copyright (C) 2023 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#pragma once
#include <stdint.h>

#include "pipeline.h"

volatile int32_t ticksToAutonomy;

#define USB_STATUS_CONNECTED 0
#define USB_STATUS_DISCONNECTED 1
uint8_t usb_status;
uint8_t io_kro_limit;
#define KBD_KRO_LIMIT 62
#define CONSUMER_KRO_LIMIT 8
#define SYSTEM_KRO_LIMIT 1

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

// state as seen by host. Need to avoid generation of reset messages when not
// needed, as well as for proper rollover state generation of keyboard report.
// These are global because we need to report current state periodically.
// NOTE: multithreading or even updating from interrupt will need locking!
uint8_t keyboard_report[MAX_KEYS + 2];
uint16_t consumer_report[CONSUMER_KRO_LIMIT];
uint8_t system_report[SYSTEM_KRO_LIMIT];


void xprintf(const char *format_p, ...); // debug printf
void ts_xprintf(const char *format_p, ...); // auto-timestamped debug printf

// Expects the message prefilled in outbox. CAUTION: Overwrites first 6 bytes!
// Wire format: [u8 cmd][u8 code][u32 systime][var payload]
void coded_timestamped_message(coded_message_t* msg);

void io_init();
void io_tick();

void io_keyboard();
void io_consumer();
void io_system();
bool io_has_space();
void io_c2(IN_c2packet_t* data);
