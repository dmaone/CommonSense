/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
*/

#pragma once
    
void usb_init(void);

void usb_send(uint8_t);
void acknowledge_command();
void process_msg(void);
void load_config(void);
