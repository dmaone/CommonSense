/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
*/
#include "globals.h"
#include "PSoC_USB.h"
#include "c2/c2_protocol.h"
#include "c2/nvram.h"
#include "scan.h"
#include <project.h>

CY_ISR(BootIRQ_ISR)
{
    Boot_Load();
}

uint32_t matrix_status[ABSOLUTE_MAX_ROWS];
uint32_t matrix_prev[ABSOLUTE_MAX_ROWS];


void printRow(uint8 row)
{
    outbox.response_type = C2RESPONSE_MATRIX_STATUS;
    outbox.payload[0] = row;
    outbox.payload[1] = config.matrixCols;
    for(uint8_t i=0; i<config.matrixCols; i++)
    {
        memcpy(outbox.payload + 2 + (i), &matrix[row][i], 1);
    }
    usb_send(OUTBOX_EP);
}

bool is_matrix_changed(void)
{
    bool retval = false;
    uint32_t current_row = 0;
    for (uint8_t i=0; i<config.matrixRows; i++)
    {
        for (uint8_t j=0; j<config.matrixCols; j++)
        {
            if (config.storage[STORAGE_ADDRESS((i*config.matrixCols) + j)] > 3u) {
                // Not a dead key in layout
#if NORMALLY_LOW
#define CMP_OP >
#else
#define CMP_OP <
#endif
                if (matrix[i][j] CMP_OP config.storage[(i*config.matrixCols) + j])
                {
                    // Above noise floor
                    current_row |= (1 << j);
                    //pings++;
                }
                else
                {
                    current_row &= ~(1 << j);
                }
            }
        }
        if (current_row != matrix_status[i]) {
            matrix_status[i] = current_row;
            retval = true;
        }
    }
    return retval;
}

uint8_t prev_report[64];
uint8_t this_report[64];

void send_report(void)
{
    // Our report is 1b modifiers 62b other keys. No LEDs.
    uint8_t count = 1; // start from second byte
    // Can't just update directly in USB buffer - host may read from there anytime.
    memset(this_report, 0x00, 64);
    for (uint8_t i=0; i<config.matrixRows; i++)
    {
        for (uint8_t j=0; j<config.matrixCols; j++)
        {
            if (config.col_params->isActive && (matrix_status[i] & (1 << j)))
            {
                // Keypress!
                uint8_t keycode = config.storage[STORAGE_ADDRESS((i*config.matrixCols)+j)];
                if ((keycode & 0xF8) == 0xE0) 
                {
                    // modifier. set the modifier bit.
                    this_report[0] |= (1 << (keycode & 7));
                } else if (keycode > 0x03) {
                    this_report[count++] = keycode;
                }
                if (!(matrix_prev[i] & (1 << j))) {
                    xprintf("KP: %d %d %d > %d", i, j, config.storage[(i*config.matrixCols) + j], matrix[i][j]);
                    xprintf("Row: %x", matrix_status[i]);
                }
            }
        }
    }
    memcpy(matrix_prev, matrix_status, sizeof(matrix_prev));
    if (memcmp(this_report, prev_report, 64) != 0) {
        //xprintf("P/T %x %x %x %x %x, %d", prev_report[0], prev_report[1], this_report[0], this_report[1], this_report[2], count);
        memcpy(prev_report, this_report, 64);
        if (!status_register.emergency_stop)
        {
            usb_keyboard_send(this_report, 64);
        }
    }
}

int main()
{
    BootIRQ_StartEx(BootIRQ_ISR);
    CyGlobalIntEnable; /* Enable global interrupts. */
    load_config();
    status_register.matrix_output = 0;
    status_register.emergency_stop = NOT_A_KEYBOARD;
    scan_init();
    scan_start(); // fill matrix state, check if ADC is operational
    CyDelay(10);
    USB_Start(0u, USB_5V_OPERATION);
    usb_init();
    memset(prev_report, 0x00, sizeof(outbox));
    for(;;)
    {
        /* Host can send double SET_INTERFACE request. */
        if (0u != USB_IsConfigurationChanged())
        {
            usb_init();
        }
        if (CTRLR_SCB.status == USB_XFER_STATUS_ACK)
        {
            process_msg((void*)CTRLR_INBOX);
            CTRLR_SCB.status = USB_XFER_IDLE;
        }
        if (KBD_SCB.status == USB_XFER_STATUS_ACK)
        {
            led_status = KBD_INBOX[0];
            KBD_SCB.status = USB_XFER_IDLE;
        }
        if (status_register.matrix_output > 0)
            for(uint8 i = 0; i<config.matrixRows; i++)
                printRow(i);
                
        if (is_matrix_changed())
        {
            send_report();
        }
    }
}

/* [] END OF FILE */
