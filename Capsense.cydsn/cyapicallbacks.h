/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#pragma once
#include "../cortex/globals.h"

/*
 * It is VERY important to include all files that define *_CALLBACK's to this
 * file.
 */

/*Define your macro callbacks here */
/*For more information, refer to the Macro Callbacks topic in the PSoC Creator
 * Help.*/
#define USB_DP_ISR_ENTRY_CALLBACK
void USB_DP_ISR_EntryCallback(void); // in PSoC_USB.c
