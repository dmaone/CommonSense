/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
*/
#include <project.h>

int main()
{
    CyGlobalIntEnable;
    // button pulls low. Not pressed - no bootloader.
    //if (1u == CyPins_ReadPin(Pins_Button)) 
    //{
    //    Bootloader_Exit(Bootloader_EXIT_TO_BTLDB);
    //}
    Bootloader_Start();
    // Never reached.
    for(;;)
    {
    }
}
