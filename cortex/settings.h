/*
 *
 * Copyright (C) 2023 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#pragma once
#include "globals.h"
#include "config.h"

void settings_send_block(OUT_c2packet_t *inbox); // Send TO device
void settings_receive_block(OUT_c2packet_t *inbox); // Receive FROM device - updates RAM copy

void settings_load(void); // Fetch from EEPROM
void settings_save(void); // Persist to EEPROM
void settings_apply(void); // apply RAM version to actual hardware

void settings_sanitize(void);
