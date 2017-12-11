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

#define EEPROM_BYTESIZE 2048
#define COMMONSENSE_BASE_SIZE 64

#ifdef MATRIX_ROWS
// Firmware. matrix dimensions compiled in.
#define COMMONSENSE_MATRIX_SIZE (MATRIX_ROWS * MATRIX_COLS)
#define COMMONSENSE_CONFIG_SIZE (COMMONSENSE_BASE_SIZE + 2 * COMMONSENSE_MATRIX_SIZE)
#endif

typedef union {
    struct {
        uint8_t configVersion;
        uint8_t matrixRows;
        uint8_t matrixCols;
        uint8_t matrixLayers;
        uint8_t capsenseFlags;
        uint8_t expMode;
        uint8_t expParam1;
        uint8_t expParam2;
        uint8_t _RESERVED0[8];
        uint16_t delayLib[NUM_DELAYS]; // 2 bytes per item!
        uint8_t layerConditions[NUM_LAYER_CONDITIONS];
        uint8_t _RESERVED1[8];
        // CONFIG SIZE - count up from here.
        // Storage is for layout-size-specifics and MUST NOT be sized here
        // because firmware can know sizes in advance, while FlightController can't.
#ifdef MATRIX_ROWS
        // Firmware.
        uint8_t thresholds[COMMONSENSE_MATRIX_SIZE];
        uint8_t layers[MATRIX_LAYERS][COMMONSENSE_MATRIX_SIZE];
        uint8_t macros[EEPROM_BYTESIZE - COMMONSENSE_CONFIG_SIZE - (MATRIX_LAYERS * COMMONSENSE_MATRIX_SIZE)];
#else
        // FlightController. Must work with what firmware tells it.
#define COMMONSENSE_CONFIG_SIZE COMMONSENSE_BASE_SIZE
        uint8_t stash[EEPROM_BYTESIZE - COMMONSENSE_CONFIG_SIZE];
#endif
    };
    uint8_t raw[EEPROM_BYTESIZE];
} psoc_eeprom_t;

#define EMPTY_FLASH_BYTE 0xff
