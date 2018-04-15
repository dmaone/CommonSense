/*
 *
 * Copyright (C) 2018 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#pragma once

#include <stdbool.h>

#include "../c2/c2_protocol.h"

#include "pipeline.h"

void serial_init(void);
void serial_nap(void);
void serial_wake(void);
void serial_send(Sup_Pdu_t* data);
void serial_tick(void);
bool serial_receive(Sup_Pdu_t* data);
void update_serial_keyboard_report(queuedScancode *key);