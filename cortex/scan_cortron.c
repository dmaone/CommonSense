/*
 *
 * Copyright (C) 2018 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <project.h>

#include "scan.h"

static volatile uint8_t current_key = 0;
static volatile uint8_t drive_code = 4;
static volatile uint8_t current_stage = 0;
//static volatile uint8_t signal = 0;

#define STROBE_START 12
#define READ_POINT STROBE_START + 2
#define STROBE_END READ_POINT + 1
#define CYCLE_END STROBE_END + 12

const uint8_t column_LUT[] = {4, 5, 8, 9, 10, 11, 12, 13, 14, 15};

CY_ISR(DriveIRQ_ISR) {
//  Strobe_Write(current_stage);
  current_stage++;
  switch (current_stage) {
    case 1: {
      Drive_Write(drive_code);
      Strobe_Write(4);
      break;
    }
    case STROBE_START:
      Strobe_Write(0);
//      signal = 0;
      break;
    case STROBE_START + 1:
      Strobe_Write(3);
      break;
    case READ_POINT:
      append_debounced(CyPins_ReadPin(SensePin_0) ? 0 : KEY_UP_MASK, current_key);
      // The range is 0 - number of keys. So at the last key we reset to zero.
      if (current_key >= 99) {
        current_key = 0;
        drive_code = 4;
      } else {
        current_key++;
      }
      drive_code++;
      // Driver generates pulses only at positions 4, 5, 8-15.
      // So we will skip non-coding positions.
      if ((drive_code & 0x0f) == 6) {
        // passed 5. Skip to 8.
        drive_code += 2;
      } else if ((drive_code & 0x0f) == 0) {
        // just passed 15. Advance 4 steps to 4.
        drive_code += 4;
      }
      break;
    case STROBE_END:
      Strobe_Write(7);
      break;
    case STROBE_END + 1:
      Strobe_Write(4);
      break;
    case CYCLE_END:
      current_stage = 0;
      break;
    default:
      break;
  }
}

CY_ISR(SenseIRQ_ISR) {
//  signal = 1;
}

void scan_init(uint8_t debouncing_period) {
  status_register &= (1 << C2DEVSTATUS_SETUP_MODE);
  scan_common_init(debouncing_period);
  DriveIRQ_StartEx(DriveIRQ_ISR);
//  SenseIRQ_StartEx(SenseIRQ_ISR);
}

void scan_reset() {
  uint8_t enableInterrupts = CyEnterCriticalSection();
  scan_common_reset();
  CyExitCriticalSection(enableInterrupts);
}

void scan_start(void) {
  scan_common_start(1);
}


void scan_nap(void) {
}

void scan_wake(void) {
}

inline void scan_tick(void) {
}
