/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#pragma once

#include <stdint.h>

#define ABSOLUTE_MAX_ROWS 16
#define ABSOLUTE_MAX_COLS 24
#define ABSOLUTE_MAX_LAYERS 8
#define NUM_LAYER_CONDITIONS 8
#define NUM_DELAYS 16
// Careful when changing above - may need to check nvram.h sizes.

#define BOOTLOADER_MAX_PACKET_LENGTH 32
#define BOOTLOADER_PAYLOAD_LENGTH (3 + BOOTLOADER_MAX_PACKET_LENGTH + 3)

/*
 * The data block for the control channel is 64 bytes, both up and down.
 * Packet format is [command byte][application-specific].
 * MESSAGES MUST BE EXACTLY 64 BYTES IN LENGTH, otherwise host will drop them.
 * If your message is longer - it is _your_ problem, you should handle it at
 * both sides. Use the IN_type_t and OUT_type_t for communication. Directions
 * are defined in USB spec sense - so IN is TO HOST, OUT is TO DEVICE. Preferred
 * method of handling messages at the host is to install event handler to TODO
 * On firmware side we should conserver resources,
 * so use case statement in dispatch_command. Do not use
 * USB_EP_INBOX_EP_ISR_ExitCallback! Messages that are NOT picked up will be
 * DROPPED at firmware side or PRINTED at the host side. They will be printed in
 * full, first byte included. This kind of restricts number of available
 * commands to 32. Should be enough for everyone!
 *
 */

enum c2command {
  C2CMD_EWO = 0x00,
  C2CMD_GET_STATUS = 0x01,
  C2CMD_ENTER_BOOTLOADER = 0x02,
  // DO NOT TOUCH THOSE THREE ABOVE!
  // Bootloader command must keep it's place or you lose firmware update
  // capability.
  C2CMD_UPLOAD_CONFIG,   // FROM host
  C2CMD_DOWNLOAD_CONFIG, // TO host
  C2CMD_APPLY_CONFIG,    // reinits sensitive parts
  C2CMD_COMMIT,
  C2CMD_ROLLBACK,
  C2CMD_SET_MODE,
  C2CMD_GET_MATRIX_STATE
};

enum c2response {
  C2RESPONSE_STATUS = 0x00,
  C2RESPONSE_CONFIG,
  C2RESPONSE_SCANCODE,
  C2RESPONSE_MATRIX_ROW
};

enum deviceStatus {
  C2DEVSTATUS_SCAN_ENABLED = 0,
  C2DEVSTATUS_OUTPUT_ENABLED,
  C2DEVSTATUS_MATRIX_MONITOR,
  C2DEVSTATUS_SETUP_MODE,
  C2DEVSTATUS_INSANE,
};

enum capsenseFlags {
  CSF_OE = 0,
  CSF_NL = 1,
};

enum deviceMode {
  C2DEVMODE_NORMAL = 0,
  C2DEVMODE_SETUP,
};

enum expMode {
  EXP_MODE_DISABLED = 0,

  EXP_MODE_SOLENOID,
  EXP_MODE_LEDS,
};

static const char *const expModeNames[] = {
    "Disabled",

    "Solenoid", "Lock LEDs",
};

typedef union {
  struct {
    uint8_t status;
    uint8_t versionMajor;
    uint8_t versionMinor;
    uint8_t dieTemp;
  } __attribute__((packed));
  uint8_t raw[4];
} device_status_t;

typedef union {
  struct {
    unsigned char response_type;
    unsigned char payload[63];
  } __attribute__((packed));
  uint8_t raw[64];
} IN_c2packet_t;

typedef union {
  struct {
    unsigned char command;
    unsigned char payload[63];
  } __attribute__((packed));
  uint8_t raw[64];
} OUT_c2packet_t;

typedef union {
  struct {
    uint8_t sop;
    uint8_t command;
    uint16_t length; // Thank Cypress for being little-endian!
    // ^^^^4 bytes
    unsigned char payload[BOOTLOADER_PAYLOAD_LENGTH]; // 1B array, 2B row, 32
                                                      // bytes data, +2B
                                                      // checksum +1b stop
                                                      // marker
  } __attribute__((packed));
  uint8_t raw[4 + BOOTLOADER_PAYLOAD_LENGTH];
} Bootloader_packet_t;

typedef union {
  struct {
    uint16_t checksum; // Thank Cypress for being little-endian!
    uint8_t eop;
  } __attribute__((packed));
  uint8_t raw[3];
} Bootloader_packet_trailer_t;

#define CONFIG_TRANSFER_BLOCK_SIZE 32
#define CONFIG_BLOCK_DATA_OFFSET 1

#define MACRO_TYPE_ONKEYUP 0x80
#define MACRO_TYPE_TAP 0x40

#define DELAYS_EVENT 0
#define DELAYS_TAP 1

// serial stuff
enum supervisory_command {
  SUP_CMD_NOOP = 0,
  SUP_CMD_KEYDOWN = 'p',
  SUP_CMD_KEYUP = 'r',
  SUP_CMD_SUSPEND = 's',
  SUP_CMD_WAKEUP = 'w',
};

typedef union {
  struct {
    char command;
    uint8_t data;
  } __attribute__((packed));
  uint8_t raw[2];
} Sup_Pdu_t;
