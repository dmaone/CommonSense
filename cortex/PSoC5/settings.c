/*
 *
 * Copyright (C) 2023 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include "../settings.h"
#include "../io.h"
#include "../scan.h"
#include "../gpio.h"
#include "../io.h"

void settings_receive_block(OUT_c2packet_t *inbox) {
  if (status_register != (1 << C2DEVSTATUS_SETUP_MODE)) {
    xprintf("Invalid status register for config upload");
    return;
  }
  uint32_t offset = inbox->payload[0] * CONFIG_TRANSFER_SIZE_USB;
  if (offset < EEPROM_BYTESIZE) {
    memcpy(config.raw + offset, inbox->payload + CONFIG_BLOCK_DATA_OFFSET,
      CONFIG_TRANSFER_SIZE_USB);
  } else {
    xprintf("Packet %d beyond NVRAM boundaries!", inbox->payload[0]);
  }
  IN_c2packet_t outbox;
  outbox.response_type = C2RESPONSE_CONFIG;
  outbox.payload[0] = inbox->payload[0];
  io_c2(&outbox);
}

void settings_send_block(OUT_c2packet_t *inbox) {
  IN_c2packet_t outbox;
  outbox.response_type = C2RESPONSE_CONFIG;
  outbox.payload[0] = inbox->payload[0];
  uint32_t offset = inbox->payload[0] * CONFIG_TRANSFER_SIZE_USB;
  if (offset < EEPROM_BYTESIZE) {
    memcpy(outbox.payload + CONFIG_BLOCK_DATA_OFFSET, config.raw + offset,
      CONFIG_TRANSFER_SIZE_USB);
  } else {
    xprintf("Packet %d beyond NVRAM boundaries!", inbox->payload[0]);
  }
  io_c2(&outbox);
}

void settings_sanitize(void) {
  if (config.matrixRows != MATRIX_ROWS || config.matrixCols != MATRIX_COLS) {
    // Yeehaw, a virgin EEPROM (or a different matrix size reflash)
    memset(config.thresholds, 0, sizeof(config.thresholds));
    config.configVersion = CS_CONFIG_VERSION;
  }
  config.matrixRows = MATRIX_ROWS;
  config.matrixCols = MATRIX_COLS;
  config.matrixLayers = MATRIX_LAYERS;
  config.switchType = SWITCH_TYPE;
  config.pedals = NUM_PEDALS;
  switch (config.adcBits) {
    case 8:
    case 10:
    case 12:
      break;
    default:
      // People don't read docs hence can't find this setting. 12 bit usually 
      // not enough to go overboard, so let's default to 12 bits and deal with
      // possible "help I'm gone 3-digit" if those ever happen.
      config.adcBits = 12; 
  };
  if (config.chargeDelay < MIN_CHARGE_DELAY) {
    config.chargeDelay = MIN_CHARGE_DELAY;
  };
  if (config.dischargeDelay < MIN_DISCHARGE_DELAY) {
    config.dischargeDelay = MIN_DISCHARGE_DELAY;
  }
  if (config.debouncingTicks < 1) {
    config.debouncingTicks = DEFAULT_DEBOUNCING_TICKS;
  }
}

void settings_load(void) {
  EEPROM_Start();
  CyDelayUs(5);
  // Copypaste from EEPROM.c/EEPROM_ReadByte! Use with caution!
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
  settings_sanitize();
  if (config.configVersion < CS_LAST_COMPATIBLE_NVRAM_VERSION) {
    status_register = (1 << C2DEVSTATUS_WRONG_NVRAM) |
                      (1 << C2DEVSTATUS_INSANE) |
                      (1 << C2DEVSTATUS_SETUP_MODE);
    xprintf("Incompatible NVRAM layout!");
    return;
  }
  if (config.configVersion != CS_CONFIG_VERSION) {
    xprintf("Old version of EEPROM - possibly unpredictable results.");
  }
}

void settings_apply(void) {
  gpio_init();
  pipeline_init(); // calls scan_reset
  scan_init(config.debouncingTicks);
  scan_start();
}

void settings_save(void) {
  settings_sanitize();
  EEPROM_Start();
  CyDelayUs(5);
  EEPROM_UpdateTemperature();
  xprintf("Updating EEPROM GO!");
  uint16 bytes_modified = 0;
  for (uint16 i = 0; i < EEPROM_BYTESIZE; i++)
    if (config.raw[i] != EEPROM_ReadByte(i)) {
      EEPROM_WriteByte(config.raw[i], i);
      bytes_modified++;
    }
  EEPROM_Stop();
  xprintf("Written %d bytes!", bytes_modified);
}
