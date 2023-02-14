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

void xprintf(const char *format_p, ...); // debug printf
void ts_xprintf(const char *format_p, ...); // auto-timestamped debug printf

// Expects the message prefilled in outbox. CAUTION: Overwrites first 6 bytes!
// Wire format: [u8 cmd][u8 code][u32 systime][var payload]
void coded_timestamped_message(coded_message_t* msg);

void io_init();
void io_tick();

void io_keyboard(void* data);
void io_consumer(void* data);
void io_system(void* data);
void io_c2(IN_c2packet_t* data);
void io_c2_blocking(IN_c2packet_t* data);
