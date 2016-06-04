/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
*/
#pragma once
#include "globals.h"

#define BootIRQ_INTERRUPT_INTERRUPT_CALLBACK
void BootIRQ_Interrupt_InterruptCallback(void);
/*Define your macro callbacks here */
    /*For more information, refer to the Macro Callbacks topic in the PSoC Creator Help.*/
#define USB_EP_8_ISR_EXIT_CALLBACK
void USB_EP_8_ISR_ExitCallback(void);    
