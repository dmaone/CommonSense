/*
 *
 * Copyright (C) 2016-2017 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
*/
#include <stdio.h>
#include <project.h>
#include "globals.h"
#include "c2/c2_protocol.h"
#include "PSoC_USB.h"
#include "scan.h"

// for xprintf - stdio + stdarg
#include <stdarg.h>

void report_status(void)
{
    outbox.response_type = C2RESPONSE_STATUS;
    outbox.payload[0] = 0;
    outbox.payload[0] |= (status_register.emergency_stop << C2DEVSTASTUS_EMERGENCY);
    outbox.payload[0] |= (status_register.matrix_output << C2DEVSTASTUS_MATRIXOUTPUT);
    outbox.payload[1] = DEVICE_VER_MAJOR;
    outbox.payload[2] = DEVICE_VER_MINOR;
    EEPROM_UpdateTemperature();
    outbox.payload[3] = dieTemperature[0];
    outbox.payload[4] = dieTemperature[1];
    usb_send(OUTBOX_EP);
    xprintf("LED status: %d %d %d %d %d", led_status&0x01, led_status&0x02, led_status&0x04, led_status&0x08, led_status&0x10);
}

void receive_config_block(OUT_c2packet_t *inbox){
    // TODO define offset via transfer block size and packet size
    memcpy(config.raw + (inbox->payload[0] * CONFIG_TRANSFER_BLOCK_SIZE), inbox->payload+31, CONFIG_TRANSFER_BLOCK_SIZE);
    outbox.response_type = C2RESPONSE_CONFIG;
    outbox.payload[0] = inbox->payload[0];
    usb_send(OUTBOX_EP);
}

void send_config_block(OUT_c2packet_t *inbox){
    outbox.response_type = C2RESPONSE_CONFIG;
    outbox.payload[0] = inbox->payload[0];
    memcpy(outbox.payload + 31, config.raw + (inbox->payload[0] * CONFIG_TRANSFER_BLOCK_SIZE), CONFIG_TRANSFER_BLOCK_SIZE);
    usb_send(OUTBOX_EP);
}

void set_hardware_parameters(void)
{
    config.capsense_flags.normallyOpen = NORMALLY_LOW;
}

void save_config(void){
    set_hardware_parameters();
    EEPROM_Start();
    CyDelayUs(5);
    EEPROM_UpdateTemperature();
    xprintf("Updating EEPROM GO!");
    uint16 bytes_modified = 0;
    for(uint16 i = 0; i < EEPROM_BYTESIZE; i++)
        if(config.raw[i] != EEPROM_ReadByte(i)) {
            EEPROM_WriteByte(config.raw[i], i);
            bytes_modified++;
        }
    EEPROM_Stop();
    xprintf("Written %d bytes!", bytes_modified);
}

void load_config(void){
    EEPROM_Start();
    CyDelayUs(5);
    // Copypaste from EEPROM.c/EEPROM_ReadByte! Use with causion!
    uint8 interruptState;
    interruptState = CyEnterCriticalSection();
    /* Request access to EEPROM for reading.
    This is needed to reserve PHUB for read operation from EEPROM */
    CyEEPROM_ReadReserve();
    memcpy(config.raw, (void *)CYDEV_EE_BASE, CYDEV_EE_SIZE);
    /* Release EEPROM array */
    CyEEPROM_ReadRelease();
    CyExitCriticalSection(interruptState);
    EEPROM_Stop();
    set_hardware_parameters();
}

void process_msg(OUT_c2packet_t * inbox)
{
    memset(outbox.raw, 0x00, sizeof(outbox));
    switch (inbox->command) {
    case C2CMD_EWO:
        status_register.emergency_stop = inbox->payload[0];
        xprintf("EWO signal received: %d", inbox->payload[0]);
        break;
    case C2CMD_GET_STATUS:
        report_status();
        break;
    case C2CMD_ENTER_BOOTLOADER:
        xprintf("Jumping to bootloader..");
        Boot_Load(); //Does not return, no need for break
    case C2CMD_UPLOAD_CONFIG:
        receive_config_block(inbox);
        break;
    case C2CMD_DOWNLOAD_CONFIG:
        send_config_block(inbox);
        break;
    case C2CMD_COMMIT:
        save_config();
        break;
    case C2CMD_ROLLBACK:
        xprintf("Resetting..");
        CySoftwareReset(); //Does not return, no need for break.
    case C2CMD_GET_MATRIX_STATE:
        status_register.matrix_output = inbox->payload[0];
        break;
    default:
        break;
    }
}

void usb_init(void)
{

    /* Wait for device to enumerate */
    while (0u == USB_GetConfiguration())
    {
        CyDelay(100);
    }
}

void usb_send(uint8_t ep)
{   
    while (USB_GetEPState(ep) & USB_NO_EVENT_ALLOWED) {}; // wait for the green light
    USB_LoadInEP(ep, outbox.raw, sizeof(outbox.raw));
    // !!!TODO!!! one can just return here if there's more than one buffer.
    while (!(USB_GetEPState(ep) & USB_IN_BUFFER_EMPTY)) {}; // wait for buffer release
}

void usb_keyboard_send(void* bufptr, uint8_t length)
{
    memcpy(KBD_OUTBOX, bufptr, length);
}

void xprintf(const char *format_p, ...)
{
    va_list va;
    va_start(va, format_p);
    vsnprintf((char *)outbox.raw, 64, format_p, va);
    va_end(va);
    usb_send(OUTBOX_EP);
} 
