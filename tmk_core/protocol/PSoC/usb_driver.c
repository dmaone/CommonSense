/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
*/
#include <stdio.h>
#include <project.h>
#include <stdbool.h>
#include "globals.h"
#include "host.h"
#include "led.h"
#include "usb_driver.h"
#include "wait.h"
//#include "config.h"
#include "c2/c2_protocol.h"

host_driver_t kbd_driver = {
    keyboard_leds,
    send_keyboard,
    send_mouse,
    send_system,
    send_consumer
};

host_driver_t *psoc_driver(void){
    return &kbd_driver;
}

void process_msg(void)
{
    switch (inbox.command) {
    case C2CMD_GET_MATRIX_STATUS:
        ephemeral_debug.matrix_output = inbox.payload[0];
        break;
    default:
        break;
    }
    acknowledge_command();
}
void usb_init(void)
{
    /* Start USB operation with 5-V operation. */
    USB_Start(0u, USB_5V_OPERATION);

    /* Wait for device to enumerate */
    while (0u == USB_GetConfiguration())
    {
        wait_ms(100);
    }
    host_set_driver(psoc_driver());
    // Start listening!
    USB_EnableOutEP(INBOX_EP);


}
/*******************************************************************************
 * Host driver 
 ******************************************************************************/
static uint8_t keyboard_led_status;
uint8_t keyboard_leds(void)
{
    return ~keyboard_led_status;
}

void USB_EP_8_ISR_ExitCallback(void)
{
    USB_ReadOutEP(INBOX_EP, inbox.raw, USB_GetEPCount(INBOX_EP));
    message_for_you_in_the_lobby = true;
}

void acknowledge_command(void)
{
    message_for_you_in_the_lobby = false;
    USB_EnableOutEP(INBOX_EP);
}

void usb_send(void)
{
    USB_LoadInEP(OUTBOX_EP, outbox.raw, sizeof(outbox.raw));
//    /* Wait for ACK after loading data. */
    while (0u == USB_GetEPAckState(OUTBOX_EP))
    {
    }
}

void send_keyboard(report_keyboard_t *report)
{
    USB_LoadInEP(1, (uint8_t *)report, sizeof(report_keyboard_t));
    /* Wait for ACK after loading data. */
    while (0u == USB_GetEPAckState(1))
    {
    }
 }

void send_mouse(report_mouse_t *report)
{
}

/* extra report structure */
typedef struct {
  uint8_t report_id;
  uint16_t usage;
} __attribute__ ((packed)) report_extra_t; 

void send_extrakeys(uint8_t report_id, uint16_t data)
{
  report_extra_t report = {
    .report_id = report_id,
    .usage = data
  };

    USB_LoadInEP(1, (uint8_t *)&report, sizeof(report_extra_t));
    /* Wait for ACK after loading data. */
    while (0u == USB_GetEPAckState(1))
    {
    }
}

void send_system(uint16_t data)
{
    send_extrakeys(2, data);
}

void send_consumer(uint16_t data)
{
    send_extrakeys(3, data);
 }
