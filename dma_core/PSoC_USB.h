/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
*/

#pragma once
#include "scan.h"

void usb_init(void);

void usb_send(uint8_t);
void usb_keyboard_send(void*, uint8_t);
void usb_wakeup(void);
void process_msg(OUT_c2packet_t *);
void load_config(void);

void keyboard_press(uint8_t);
void keyboard_release(uint8_t);
void update_keyboard_mods(uint8_t);
