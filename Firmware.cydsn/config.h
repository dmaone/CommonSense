/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
*/
#pragma once
    
#define VENDOR_ID       0xFEED
#define PRODUCT_ID      0xBEEF
#define DEVICE_VER_MAJOR      0x00
#define DEVICE_VER_MINOR      0x01
#define MANUFACTURER    DMA Labs
#define PRODUCT         CommonSense
#define DESCRIPTION     Universal capsense controller
// those above are not used because of the way USB component is configured in PSoC creator.
// Sorry, GUI only :(

#define MATRIX_ROWS 16
#define MATRIX_COLS 32

#define EXTRAKEY_ENABLE
#define DEBUG_EP 2
