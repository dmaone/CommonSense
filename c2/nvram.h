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

#define CS_CONFIG_VERSION 2

// Output Enabled
#define CSF_OE_SHIFT 0
#define CSF_OE_MASK 1

// Normally Low
#define CSF_NL_SHIFT 1
#define CSF_NL_MASK 2

#define EEPROM_BYTESIZE 2048
#define CONFIG_TRANSFER_BLOCK_SIZE 32

typedef union {
    struct {
        uint8_t configVersion;
        uint8_t matrixRows;
        uint8_t matrixCols;
        uint8_t layerCount;
        uint8_t capsenseFlags;
        uint8_t Reserved[11];
        // CONFIG SIZE - count up from here.
        // Storage is for layout-size-specifics and MUST NOT be sized here
        // because firmware can know sizes in advance, while FlightController can't.
#ifdef MATRIX_ROWS
        uint8_t noiseFloor[MATRIX_ROWS][MATRIX_COLS];
        uint8_t noiseCeiling[MATRIX_ROWS][MATRIX_COLS];
#define COMMONSENSE_CONFIG_SIZE (16 + 2 * MATRIX_ROWS * MATRIX_COLS)
        uint8_t lmstash[EEPROM_BYTESIZE - COMMONSENSE_CONFIG_SIZE];
#else
#define COMMONSENSE_CONFIG_SIZE 16
        uint8_t stash[EEPROM_BYTESIZE - COMMONSENSE_CONFIG_SIZE];
#endif
    };
    uint8_t raw[EEPROM_BYTESIZE];
} psoc_eeprom_t;
