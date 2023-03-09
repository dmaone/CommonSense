/*
 *
 * Copyright (C) 2023 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * HID report conversion routines
 */

#pragma once
#include <stdint.h>

#include "pipeline.h"
#include "io.h"

void hid_init();
void hid_reset_reports();
void hid_update_keyboard(hid_event* key);
void hid_update_consumer(hid_event* key);
void hid_update_system(hid_event* key);
