/*
 *
 * Copyright (C) 2016-2017 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#pragma once
#include "globals.h"
#include <project.h>

typedef union {
  struct {
    uint32_t sysTime;
    uint8_t flags;
    uint8_t keycode;
  } __attribute__((packed));
  uint8_t raw[6];
} queuedScancode;

#define USBCODE_TRANSPARENT 0
#define USBCODE_NOEVENT 1
#define USBCODE_ERO 1
#define USBCODE_EXP_TOGGLE 3
#define USBCODE_A 4

#define USBQUEUE_RELEASED_MASK 0x80
#define USBQUEUE_REAL_KEY_MASK 0x40

#define KEYCODE_BUFFER_END 63
#define KEYCODE_BUFFER_NEXT(X) ((X + 1) & KEYCODE_BUFFER_END)
#define KEYCODE_BUFFER_PREV(X) ((X + KEYCODE_BUFFER_END) & KEYCODE_BUFFER_END)
// ^^^ THIS MUST EQUAL 2^n-1!!! Used as bitmask.

#define MACRO_NOT_FOUND UINT_FAST16_MAX
#define MACRO_KEY_UPDOWN_RELEASE 0x02

queuedScancode USBQueue[KEYCODE_BUFFER_END + 1];
uint8_t USBQueue_readpos;
uint8_t USBQueue_writepos;
#define USBQUEUE_IS_EMPTY                                                      \
  (USBQueue_readpos == USBQueue_writepos &&                                    \
   USBQueue[USBQueue_readpos].keycode == USBCODE_NOEVENT)

uint8_t mods;
uint8_t layerMods;
#define LAYER_MODS_SHIFT 4
uint8_t currentLayer;

uint16_t cooldown_timer;

void pipeline_init(void);
void pipeline_process(void);
bool pipeline_process_wakeup(void);
