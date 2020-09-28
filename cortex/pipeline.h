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
    uint8_t code;
    uint16_t reserved_;
  } __attribute__((packed));
  uint64_t raw;
} hid_event;

uint8_t mods;
uint8_t layerMods;
#define LAYER_MODS_SHIFT 4
uint8_t currentLayer;

uint16_t cooldown_timer;

void pipeline_init(void);
void pipeline_process(void);
bool pipeline_process_wakeup(void);
