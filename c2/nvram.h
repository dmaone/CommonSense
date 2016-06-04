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

typedef union {
    struct {
        bool outputEnabled:1;
        bool normallyOpen:1;
        bool interlacedScan:1;
        uint8_t reserved:4;
        uint8_t thresholdVoltage;
    };
    uint16_t raw;
} capsense_settings_t;

typedef union {
    struct {
        bool isActive:1;
        uint8_t reserved:3;
        uint8_t rowNumber:4;
    };
    uint8_t raw;
} row_settings_t;

typedef union {
    struct {
        bool isActive:1;
        uint8_t reserved:2;
        uint8_t colNumber:5;
    };
    uint8_t raw;
} col_settings_t;

#define COMMONSENSE_CONFIG_SIZE 64
#define CHIP_EEPROM_SIZE 2048
#define READOUT_DELAY_OFFSET 2

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
        uint8_t commonsense_reserved[1];
        row_settings_t row_params[8];
        // 32 bytes boundary
        col_settings_t col_params[32];
        uint8_t storage[(CHIP_EEPROM_SIZE - COMMONSENSE_CONFIG_SIZE)];
    };
    unsigned char raw[CHIP_EEPROM_SIZE];
} psoc_eeprom_t;