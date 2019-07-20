/*
 *
 * Copyright (C) 2018 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <project.h>

#include "scanner_sun.h"

#include "scan_common.h"
uint8_t local_led_status;

void sync_leds(void) {
  local_led_status = led_status;
  N_UART_PutChar(0x0e);
  uint8_t led_map = 0;
  led_map |= (TEST_BIT(led_status, 2)) ? 4 : 0;
  led_map |= (TEST_BIT(led_status, 1)) ? 8 : 0;
  led_map |= (TEST_BIT(led_status, 0)) ? 1 : 0;
  N_UART_PutChar(led_map);
}

void scan_init(uint8_t debouncing_period) {
  N_UART_Start();
  N_UART_PutChar(0x01);
  SET_BIT(status_register, C2DEVSTATUS_OUTPUT_ENABLED);
  SET_BIT(status_register, C2DEVSTATUS_SCAN_ENABLED);
}

void scan_start(void) {
  sync_leds();
  init_scancodes();
}

void scan_reset(void) {
  sync_leds();
  init_scancodes();
}

void scan_tick(void) {
  if (local_led_status != led_status) {
    sync_leds();
  }
  uint8_t code = N_UART_GetChar();
  if (!code) return;
  switch(code) {
      case 0xFF:  // reset success: FF 04
          CyDelay(50);
          code = N_UART_GetChar();
          if (code == 0x04) {
            sync_leds();
          }
          return;
      case 0xFE:  // layout: FE <layout>
          CyDelay(50);
          code = N_UART_GetChar();
          return;
      case 0x7E:  // reset fail: 7E 01
          CyDelay(50);
          code = N_UART_GetChar();
          return;
      case 0x7F: // all keys up
          append_scancode(KEY_UP_MASK, COMMONSENSE_NOKEY);
          return;
      default:
        append_scancode((code & KEY_UP_MASK), (code & SCANCODE_MASK));
  }
  // Uncomment below for click
  // N_UART_PutChar(0x0A);

}

void scan_nap(void) {
}

void scan_wake(void) {
}

void scan_sanity_check(void) {
}

void report_matrix_readouts(void) {
}
