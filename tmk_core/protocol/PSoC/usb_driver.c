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
#include "host.h"
#include "led.h"
#include "usb_driver.h"
#include "wait.h"
#include "config.h"


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
    USB_EnableOutEP(8);


}
/*******************************************************************************
 * Host driver 
 ******************************************************************************/
static uint8_t keyboard_led_status;
static uint8_t keyboard_leds(void)
{
    return ~keyboard_led_status;
}

void USB_EP_8_ISR_ExitCallback(void)
{
    USB_ReadOutEP(2, &keyboard_led_status, 1);
    USB_EnableOutEP(2);
}

static void send_debug(const char* str, uint8_t len)
{
    USB_LoadInEP(DEBUG_EP, (unsigned char*)str, len);
    /* Wait for ACK after loading data. */
    while (0u == USB_GetEPAckState(DEBUG_EP))
    {
    }
}

static void send_keyboard(report_keyboard_t *report)
{
    USB_LoadInEP(1, (uint8_t *)report, sizeof(report_keyboard_t));
    /* Wait for ACK after loading data. */
    while (0u == USB_GetEPAckState(1))
    {
    }
 }

static void send_mouse(report_mouse_t *report)
{
}

/* extra report structure */
typedef struct {
  uint8_t report_id;
  uint16_t usage;
} __attribute__ ((packed)) report_extra_t; 

static void send_extrakeys(uint8_t report_id, uint16_t data)
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

static void send_system(uint16_t data)
{
    send_extrakeys(2, data);
}

static void send_consumer(uint16_t data)
{
    send_extrakeys(3, data);
 }
