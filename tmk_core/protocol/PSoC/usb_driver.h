/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
*/

#pragma once
    
#include <project.h>
#include "host_driver.h"

/* Host driver */
host_driver_t *psoc_driver(void);

void usb_init(void);

void usb_send(void);
void acknowledge_command();
void process_msg(void);
void load_config(void);

void send_keyboard(report_keyboard_t *report);
void send_mouse(report_mouse_t *report);
void send_system(uint16_t data);
void send_consumer(uint16_t data);
void send_debug(const char* str, uint8_t len);
