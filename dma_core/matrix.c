/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
*/

#include "matrix.h"
#include "globals.h"
#include "usb_driver.h"

uint8_t matrix_rows(void)
{
    return config.matrixRows;
}

uint8_t matrix_cols(void)
{
    return config.matrixCols;
}

uint8_t matrix_scan(void)
{
    if (message_for_you_in_the_lobby)
    {
        process_msg();
    }
    return 0;
}

matrix_row_t matrix_get_row(uint8_t row)
{
    return 0;
}

void matrix_print(void)
{
}