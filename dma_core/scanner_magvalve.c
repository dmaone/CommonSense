/*
 *
 * Copyright (C) 2018 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <project.h>

#include "scanner_magvalve.h"
#include "scan_common.h"

CY_ISR_PROTO(Result_ISR);

uint8_t current_row;
uint8_t mux_position;
uint8_t key_index;
bool scan_in_progress;

#define SCAN_TRIGGER (1 << 4)

void sensor_init() {
  Cmp0_Start();
  Cmp1_Start();
  Cmp2_Start();
  Cmp3_Start();
  
  VDAC0_Start();
  VDAC1_Start();
  VDAC2_Start();
  VDAC3_Start();

  VDAC0_SetValue(config.thresholds[0]);
  VDAC1_SetValue(config.thresholds[1]);
  VDAC2_SetValue(config.thresholds[2]);
  VDAC3_SetValue(config.thresholds[3]);
  
  SetupDelay_Start();
  SetupDelay_WritePeriod(config.chargeDelay);
  
  ResetDelay_Start();
  ResetDelay_WritePeriod(config.dischargeDelay);

  CyDelayUs(1); // Wait for VDAC outputs to stabilize
  uint16_t cal0 = Cmp0_ZeroCal();
  uint16_t cal1 = Cmp1_ZeroCal();
  uint16_t cal2 = Cmp2_ZeroCal();
  uint16_t cal3 = Cmp3_ZeroCal();
  xprintf("Thr: %2d %2d %2d %2d\nCal: %2d %2d %2d %2d",
      config.thresholds[0], config.thresholds[1],
      config.thresholds[2], config.thresholds[3],
      cal0, cal1, cal2, cal3);

  ResultIRQ_StartEx(Result_ISR);
}

void sensor_nap(void) {
  SetupDelay_Sleep();
  ResetDelay_Sleep();

  Cmp0_Sleep();
  Cmp1_Sleep();
  Cmp2_Sleep();
  Cmp3_Sleep();
  
  VDAC0_Sleep();
  VDAC1_Sleep();
  VDAC2_Sleep();
  VDAC3_Sleep();
}

void sensor_wake(void) {
  SetupDelay_Wakeup();
  ResetDelay_Wakeup();

  Cmp0_Wakeup();
  Cmp1_Wakeup();
  Cmp2_Wakeup();
  Cmp3_Wakeup();
  
  VDAC0_Wakeup();
  VDAC1_Wakeup();
  VDAC2_Wakeup();
  VDAC3_Wakeup();
}

CY_ISR(Result_ISR) {
  // Read current results, then setup next scan.
  uint8_t status = SensorStatus_Read();
  
  // In the name of Performance, things are scanned backwards.
  // So, in the name of Consistency, we'll use channels backwards too.
  // We are screwed at pinout level anyway, so let's keep madness in one place.
  append_debounced(TEST_BIT(status, 3) ? 0 : KEY_UP_MASK, --key_index);
  append_debounced(TEST_BIT(status, 2) ? 0 : KEY_UP_MASK, --key_index);
  append_debounced(TEST_BIT(status, 1) ? 0 : KEY_UP_MASK, --key_index);
  append_debounced(TEST_BIT(status, 0) ? 0 : KEY_UP_MASK, --key_index);

  VDAC3_SetValue(config.thresholds[key_index - 1]);
  VDAC2_SetValue(config.thresholds[key_index - 2]);
  VDAC1_SetValue(config.thresholds[key_index - 3]);
  VDAC0_SetValue(config.thresholds[key_index - 4]);

  if (mux_position == 0) {
    mux_position = MATRIX_COLS / 4;
    if (current_row == 0) {
      // End of the scan pass. Loop if full throttle, otherwise stop.
      if (power_state != DEVSTATE_FULL_THROTTLE
          || 0 == TEST_BIT(status_register, C2DEVSTATUS_SCAN_ENABLED)) {
        scan_in_progress = false;
        return;
      }
      current_row = MATRIX_ROWS;
    }
    key_index = current_row * MATRIX_COLS;
    current_row--;
    // Set up the row - this doesn't trigger anything yet. Also not reset ever.
    DriveReg0_Write(1 << current_row);
  }
  SensorReg_Write((1 << (--mux_position)) + SCAN_TRIGGER);
}

void scan_init(uint8_t debouncing_period) {
  status_register &= (1 << C2DEVSTATUS_SETUP_MODE);
  while (scan_in_progress) {}; // Make sure scan is stopped.
  scan_common_init(debouncing_period);
  sensor_init();
}

void scan_reset() {
  uint8_t enableInterrupts = CyEnterCriticalSection();
  scan_common_reset();
  CyExitCriticalSection(enableInterrupts);
}

void scan_start(void) {
  if (scan_in_progress) {
    return;
  }
  scan_common_start();

  // Set things into "end of the cycle" position, then let magic happen.
  // Should cause zeroes in first readout because of trigger value below.
  mux_position = 0;
  current_row = 0;
  key_index = MATRIX_COLS; // Guard value, treat as random.
  SensorReg_Write(SCAN_TRIGGER); // First run with grounded inputs.
  scan_in_progress = true;
}

void scan_nap(void) {
  while (scan_in_progress) {} // Unsafe to proceed with interrupts flying around
  sensor_nap();
}

void scan_wake(void) {
  sensor_wake();
}

inline void scan_tick(void) {}
