/*
 * NVRAM memory layout - so flight controller and firmware both know what the
 * format is.
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#pragma once

#include "c2_protocol.h"
#include <stdbool.h>
#include <stdint.h>

#define CS_CONFIG_VERSION 3
#define CS_LAST_COMPATIBLE_NVRAM_VERSION 3

#define EEPROM_BYTESIZE 2048
#define COMMONSENSE_BASE_SIZE 64

#define DEFAULT_DEBOUNCING_TICKS 4

typedef union {
  struct {
    uint8_t configVersion; // +0
    uint8_t matrixRows;
    uint8_t matrixCols;
    uint8_t matrixLayers;
    uint8_t pedals; // +4
    uint8_t expMode;
    uint8_t expParam1;
    uint8_t expParam2;
    uint8_t adcBits; // +8
    uint8_t chargeDelay;
    uint16_t dischargeDelay;
    uint8_t debouncingTicks; // +12
    uint8_t pedalDebouncingTicks;
    uint8_t _RESERVED0[2];
    uint16_t delayLib[NUM_DELAYS]; // 2 bytes per item!
    uint8_t layerConditions[ABSOLUTE_MAX_LAYERS];
    uint8_t switchType;
    uint8_t _RESERVED1[7];
// CONFIG SIZE - count up from here.
// Storage is for layout-size-specifics and MUST NOT be sized here
// because firmware can know sizes in advance, while FlightController can't.
#ifdef MATRIX_ROWS
    // Firmware. matrix dimensions compiled in.
#define COMMONSENSE_MATRIX_SIZE (MATRIX_ROWS * MATRIX_COLS)
    uint8_t thresholds[COMMONSENSE_MATRIX_SIZE];
    uint8_t layers[COMMONSENSE_MATRIX_SIZE][MATRIX_LAYERS];
#if NUM_PEDALS > 0
#if NUM_PEDALS + COMMONSENSE_MATRIX_SIZE > 254
#error max keyIndex too large - reduce matrix size or number of pedals!
#endif
#define COMMONSENSE_NUM_KEYS (COMMONSENSE_MATRIX_SIZE + NUM_PEDALS)
    uint8_t pedal_layers[NUM_PEDALS * MATRIX_LAYERS];
    uint8_t pedal_flags[NUM_PEDALS];
#endif
    uint8_t macros[EEPROM_BYTESIZE -
                   COMMONSENSE_MATRIX_SIZE - // thresholds
                   COMMONSENSE_MATRIX_SIZE * MATRIX_LAYERS - // layout
                   NUM_PEDALS * MATRIX_LAYERS - // pedals layout
                   NUM_PEDALS]; // pedal flags
#else
// FlightController. Must work with what firmware tells it.
    uint8_t stash[EEPROM_BYTESIZE - COMMONSENSE_BASE_SIZE];
#endif
  };
  uint8_t raw[EEPROM_BYTESIZE];
} psoc_eeprom_t;


#define EMPTY_FLASH_BYTE 0xff
