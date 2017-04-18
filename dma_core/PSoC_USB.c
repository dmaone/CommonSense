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

// for xprintf - stdio + stdarg
#include <stdarg.h>

CY_ISR_PROTO(Suspend_ISR);

void report_status(void)
{
    memset(outbox.raw, 0, sizeof(outbox));
    outbox.response_type = C2RESPONSE_STATUS;
    outbox.payload[0] = (status_register.emergency_stop << C2DEVSTATUS_EMERGENCY)
                      | (status_register.matrix_output << C2DEVSTATUS_MATRIX_OUTPUT)
                      | (status_register.setup_mode << C2DEVSTATUS_SETUP_MODE);
    outbox.payload[1] = DEVICE_VER_MAJOR;
    outbox.payload[2] = DEVICE_VER_MINOR;
    EEPROM_UpdateTemperature();
    outbox.payload[3] = dieTemperature[0];
    outbox.payload[4] = dieTemperature[1];
    usb_send_c2();
    xprintf("time: %d", systime);
    //xprintf("LED status: %d %d %d %d %d", led_status&0x01, led_status&0x02, led_status&0x04, led_status&0x08, led_status&0x10);
}

void receive_config_block(OUT_c2packet_t *inbox){
    // TODO define offset via transfer block size and packet size
    memcpy(
        config.raw + (inbox->payload[0] * CONFIG_TRANSFER_BLOCK_SIZE),
        inbox->payload + CONFIG_BLOCK_DATA_OFFSET,
        CONFIG_TRANSFER_BLOCK_SIZE
    );
    memset(outbox.raw, 0, sizeof(outbox));
    outbox.response_type = C2RESPONSE_CONFIG;
    outbox.payload[0] = inbox->payload[0];
    usb_send_c2();
}

void send_config_block(OUT_c2packet_t *inbox){
    memset(outbox.raw, 0, sizeof(outbox));
    outbox.response_type = C2RESPONSE_CONFIG;
    outbox.payload[0] = inbox->payload[0];
    memcpy(
        outbox.payload + CONFIG_BLOCK_DATA_OFFSET,
        config.raw + (inbox->payload[0] * CONFIG_TRANSFER_BLOCK_SIZE),
        CONFIG_TRANSFER_BLOCK_SIZE
    );
    usb_send_c2();
}

void set_hardware_parameters(void)
{
    config.capsenseFlags = FORCE_BIT(config.capsenseFlags, CSF_NL, NORMALLY_LOW);
    config.matrixRows = MATRIX_ROWS;
    config.matrixCols = MATRIX_COLS;
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
    if (config.configVersion != CS_CONFIG_VERSION)
    {
        // Unexpected config version - not sure calibration data are there!
        config.capsenseFlags = FORCE_BIT(config.capsenseFlags, CSF_OE, 0);
        status_register.emergency_stop = true;
    }
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

void process_msg(OUT_c2packet_t * inbox)
{
    memset(outbox.raw, 0x00, sizeof(outbox));
    switch (inbox->command) {
    case C2CMD_EWO:
        status_register.emergency_stop = inbox->payload[0];
        xprintf("EWO signal received: %d", inbox->payload[0]);
        scan_reset();
        break;
    case C2CMD_GET_STATUS:
        report_status();
        break;
    case C2CMD_SET_MODE:
        status_register.setup_mode = inbox->payload[0];
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
        scan_reset();
        break;
    default:
        break;
    }
}

void usb_send_c2(void)
{   
    USB_WAIT_FOR_IN_EP(OUTBOX_EP);
    USB_LoadInEP(OUTBOX_EP, outbox.raw, sizeof(outbox.raw));
}

void update_keyboard_mods(uint8_t mods)
{
    KBD_OUTBOX[0] = mods;
    USB_SEND_REPORT(KBD);
}

inline void keyboard_press(uint8_t keycode)
{
    for (uint8_t cur_pos = 2; cur_pos < 2 + KRO_LIMIT; cur_pos++)
    {
        if (KBD_OUTBOX[cur_pos] == keycode)
        {
            xprintf("Existing %d pos %d", keycode, cur_pos);
            break;
        }
        else if (KBD_OUTBOX[cur_pos] == 0)
        {
            KBD_OUTBOX[cur_pos] = keycode;
            //xprintf("Pressed %d pos %d", keycode, cur_pos);
            break;
        }
    }
}

inline void keyboard_release(uint8_t keycode)
{
    uint8_t cur_pos;
    bool move = false;
    for (cur_pos = 2; cur_pos < 2 + KRO_LIMIT; cur_pos++)
    {
        if (move)
        {
            KBD_OUTBOX[cur_pos - 1] = KBD_OUTBOX[cur_pos];
        }
        else if (KBD_OUTBOX[cur_pos] == keycode)
        {
            move = true;
        }
    }
    if (move)
    {
        // Key was, in fact, pressed.
        KBD_OUTBOX[2 + KRO_LIMIT] = 0;
        //xprintf("Released %d", keycode);
    }
}

void update_keyboard_report(queuedScancode *key)
{
    //xprintf("Updating report for %d", key->keycode);
    if ((key->flags & USBQUEUE_RELEASED_MASK) == 0)
    {
        keyboard_press(key->keycode);
    }
    else
    {
        keyboard_release(key->keycode);
    }
    USB_SEND_REPORT(KBD);
}

// Report consists of 16 bit values - so I kind of know what I'm doing here.
static uint16_t *consumer_outbox = (uint16_t *)&CONSUMER_OUTBOX;

const uint16_t consumer_mapping[16] = {
    0xcd, // Play/pause
    0xe2, // Mute
    0xe9, // Vol++
    0xea, // Vol--
    0xb8, // Eject
    0x00,
    0x00,
    0x00,
//8    
    0xb0, // Play
    0xb1, // Pause
    0xb2, // Record
    0xb3, // Fwd
    0xb4, // Rev
    0xb5, // NTrk
    0xb6, // PTrk
    0xb7  // Stop
//16
};

static inline void consumer_press(uint16_t keycode)
{
    for (uint8_t cur_pos = 0; cur_pos < CONSUMER_KRO_LIMIT; cur_pos++)
    {
        if (consumer_outbox[cur_pos] == keycode)
        {
            xprintf("Existing %d pos %d", keycode, cur_pos);
            break;
        }
        else if (consumer_outbox[cur_pos] == 0)
        {
            consumer_outbox[cur_pos] = keycode;
            //xprintf("Pressed %d pos %d", keycode, cur_pos);
            break;
        }
    }
    //xprintf("C_Pressing %d", keycode);
}

static inline void consumer_release(uint16_t keycode)
{
    uint8_t cur_pos;
    bool move = false;
    for (cur_pos = 0; cur_pos < CONSUMER_KRO_LIMIT; cur_pos++)
    {
        if (move)
        {
            consumer_outbox[cur_pos - 1] = consumer_outbox[cur_pos];
        }
        else if (consumer_outbox[cur_pos] == keycode)
        {
            move = true;
        }
    }
    if (move)
    {
        // Key was, in fact, pressed.
        consumer_outbox[CONSUMER_KRO_LIMIT] = 0;
        //xprintf("C_Released %d", keycode);
    }
}

void update_consumer_report(queuedScancode *key)
{
    //xprintf("Updating report for %d", key->keycode);
    uint16_t keycode = consumer_mapping[key->keycode - 0xe8];
    if ((key->flags & USBQUEUE_RELEASED_MASK) == 0)
    {
        consumer_press(keycode);
    }
    else
    {
        consumer_release(keycode);
    }
    USB_SEND_REPORT(CONSUMER);
}

void update_system_report(queuedScancode *key)
{
    uint8_t key_index = key->keycode - 0xa5;
    if ((key->flags & USBQUEUE_RELEASED_MASK) == 0)
    {
        SYSTEM_OUTBOX[0] |= (1 << key_index);
    }
    else
    {
        SYSTEM_OUTBOX[0] &= ~(1 << key_index);
    }
    xprintf("System: %d", SYSTEM_OUTBOX[0]);
    USB_SEND_REPORT(SYSTEM);
}

void usb_suspend_monitor_start(void)
{
    SuspendWD_Stop();
    SuspendWD_WriteCounter(0);
    SuspendWD_Start();
    USBSuspendIRQ_StartEx(Suspend_ISR); // Does disabling for you, safe to use that way.
}

void usb_suspend_monitor_stop(void)
{
    SuspendWD_Stop();
    USBSuspendIRQ_Stop();
}

void usb_init(void)
{
    USB_Start(0u, USB_5V_OPERATION);
    power_state = DEVSTATE_FULL_THROTTLE;
}

void usb_configure(void)
{
    /* clear results */
    memset(KBD_OUTBOX, 0, sizeof(KBD_OUTBOX));
    memset(CONSUMER_OUTBOX, 0, sizeof(CONSUMER_OUTBOX));
    memset(SYSTEM_OUTBOX, 0, sizeof(SYSTEM_OUTBOX));
    /* Wait for device to enumerate */
    while (0u == USB_GetConfiguration()) {};
    usb_suspend_monitor_start();
}

void nap(void)
{
    // TODO reconfigure monitor period to provide periodic wakeups for monitor-in-suspend
    usb_suspend_monitor_stop();
    uint8_t rwu = USB_RWUEnabled();
    USB_Suspend();
    if (rwu == 0)
    {
        power_state = DEVSTATE_SLEEP;
        CyPmAltAct(PM_ALT_ACT_TIME_NONE, PM_ALT_ACT_SRC_NONE);
    }
    else
    {
        power_state = DEVSTATE_WATCH;
        //SetFreq hangs us dry.
        //CyIMO_SetFreq(CY_IMO_FREQ_24MHZ);
// CyFlash_SetWaitCycles(3);
// CyDelayFreq(3000000); - not supposed to be used, busy loops are evil
    }
}

void wake(void)
{
    USB_Resume();
    power_state = DEVSTATE_FULL_THROTTLE;
    scan_start();
    usb_suspend_monitor_start();
    //CyIMO_SetFreq(CY_IMO_FREQ_USB);
}

void usb_send_wakeup(void)
{
    CyDelay(5); // Just in case, not to violate spec by waking immediately.
    wake();
    usb_suspend_monitor_stop();
    USB_Force(USB_FORCE_K);
    CyDelay(5);
    USB_Force(USB_FORCE_NONE);
    /*
     * Host must send resume for at least 20ms (USB 2.0 spec 7.1.7.7).
     * So, 15 more.
     * We also officially have 10ms to wake.
     * Let's sit here a bit longer
     * so we don't have to worry about suspend watchdog shutting us down.
     */
    CyDelay(15 + 2);
    usb_suspend_monitor_start();
}

CY_ISR(Suspend_ISR)
{
#ifdef DEBUG_INTERRUPTS
    PIN_DEBUG(1, 5)
#endif
    if (power_state == DEVSTATE_SUSPENDING)
    {
        return;
    }
    //"USB_Dp_Read()" != 0 && USB_Dm_Read() == 0
    if ((USB_Dp_PS & (USB_Dp__MASK | USB_Dm__MASK)) == USB_Dp__MASK)
    {
        // Suspend is when no activity for 3ms and J (=Dp is high)
        power_state = DEVSTATE_SUSPENDING;
    }
    // bus reset while awake is handled by component.
    // suspend state is handled by DP ISR
}

void USB_DP_ISR_EntryCallback(void)
{
#ifdef DEBUG_INTERRUPTS
    PIN_DEBUG(1, 4)
#endif
    if (power_state == DEVSTATE_RESUMING)
    {
        return;
    }
    power_state = DEVSTATE_RESUMING;
}

void xprintf(const char *format_p, ...)
{
    va_list va;
    va_start(va, format_p);
    vsnprintf((char *)outbox.raw, sizeof(outbox), format_p, va);
    va_end(va);
    usb_send_c2();
}
