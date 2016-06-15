/*
 * NVRAM memory layout - so flight controller and firmware both know what the format is.
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "c2_protocol.h"

typedef union {
    struct {
        bool outputEnabled:1;
        bool normallyOpen:1;
        bool interlacedScan:1;
        uint8_t reserved:4;
    };
    uint8_t raw;
} capsense_settings_t;

typedef union {
    struct {
        uint8_t rowNumber:4;
        uint8_t reserved:3;
        bool isActive:1;
    };
    uint8_t raw;
} row_settings_t;

typedef union {
    struct {
        uint8_t colNumber:5;
        uint8_t reserved:2;
        bool isActive:1;
    };
    uint8_t raw;
} col_settings_t;

#define COMMONSENSE_CONFIG_SIZE 72
#define EEPROM_BYTESIZE 2048
#define CONFIG_TRANSFER_BLOCK_SIZE 32
#define STORAGE_SIZE (EEPROM_BYTESIZE - COMMONSENSE_CONFIG_SIZE)
#define READOUT_DELAY_OFFSET 2
#define STORAGE_ADDRESS(X) (STORAGE_SIZE - (X) - 1)
typedef union {
    struct {
        uint8_t configVersion;
        uint8_t debug;
        uint8_t default_layer;
        uint8_t keyswaps;
        uint8_t mouse_accel;
        uint8_t backlight;
        uint8_t tmk_reserved[11]; //reserve for future TMK config info
        // 16 bytes boundary
        uint8_t matrixRows;
        uint8_t matrixCols;
        uint8_t macroCount;
        uint8_t layerCount;
        uint8_t readoutDelay; // nanoseconds, phase lag of analog clock. max. is 10.
        capsense_settings_t capsense_flags;
        uint8_t thresholdVoltage;
        uint8_t commonsense_reserved[1];
        // Only .raw is used because bit fields suck.
        // TODO: change to uint8_t and forget about bit fields for storable stuff.
        // NOTE: Looks like got it working - first bit is high bit.
        row_settings_t row_params[ABSOLUTE_MAX_ROWS];
        col_settings_t col_params[ABSOLUTE_MAX_COLS];
        // -- CONFIG SIZE. Storage is for user-defined stuff like layouts.
        uint8_t storage[STORAGE_SIZE];
    };
    uint8_t raw[EEPROM_BYTESIZE];
} psoc_eeprom_t;