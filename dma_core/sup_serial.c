/*
 *
 * Copyright (C) 2018 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <project.h>
#include "sup_serial.h"

#define BLE_BUFFER_END 31
#define BLE_BUFFER_NEXT(X) ((X + 1) & BLE_BUFFER_END)
#define BLE_BUFFER_PREV(X) ((X + BLE_BUFFER_END) & BLE_BUFFER_END)
// ^^^ THIS MUST EQUAL 2^n-1!!! Used as bitmask.

Sup_Pdu_t SCQueue[32];
uint8_t SCQueueReadPos = 0;
uint8_t SCQueueWritePos = 0;
Sup_Pdu_t i2c_inbox;
Sup_Pdu_t i2c_outbox = {{255, 255}};

// None of this happens in ISR, so we can be sloppy
inline void queue_ble_command(Sup_Pdu_t *cmd) {
  SCQueueWritePos = BLE_BUFFER_NEXT(SCQueueWritePos);
  SCQueue[SCQueueWritePos] = *cmd;
}

void serial_init(void) {
  Sup_I2C_SlaveInitReadBuf(i2c_outbox.raw, sizeof(i2c_outbox));
  Sup_I2C_SlaveInitWriteBuf(i2c_inbox.raw, sizeof(i2c_inbox));
  Sup_I2C_Start();
}

void serial_nap(void) {
  Sup_I2C_Sleep();
}

void serial_wake(void) {
  Sup_I2C_Wakeup();
}

void serial_send(Sup_Pdu_t* packet) {
  queue_ble_command(packet);
}

void serial_tick(void) {
  if (Sup_I2C_SlaveStatus() & Sup_I2C_SSTAT_WR_CMPLT) {
    if (Sup_I2C_SlaveGetWriteBufSize() == sizeof(i2c_inbox)) {
      uint8_t enableInterrupts = CyEnterCriticalSection();
      Sup_I2C_SlaveClearWriteStatus();
      Sup_I2C_SlaveClearWriteBuf();
      CyExitCriticalSection(enableInterrupts);
      //xprintf("Rcvd %d %d", i2c_inbox.command, i2c_inbox.data);
      if (i2c_inbox.command == SUP_CMD_SUSPEND 
          && power_state == DEVSTATE_FULL_THROTTLE) {
        power_state = DEVSTATE_SLEEP_REQUEST;
      }
    }
  }
  if (SCQueueReadPos != SCQueueWritePos) {
    if (Sup_I2C_SlaveStatus() & Sup_I2C_SSTAT_RD_CMPLT) {
      if (Sup_I2C_SlaveGetReadBufSize() == 2) {
        SCQueueReadPos = BLE_BUFFER_NEXT(SCQueueReadPos);
        i2c_outbox = SCQueue[SCQueueReadPos];
        uint8_t enableInterrupts = CyEnterCriticalSection();
        Sup_I2C_SlaveClearReadBuf();
        Sup_I2C_SlaveClearReadStatus();
        CyExitCriticalSection(enableInterrupts);
        //xprintf("Sent %d %d", i2c_outbox.command, i2c_outbox.data);
      }
    }
  }
}

void update_serial_keyboard_report(queuedScancode *key) {
  Sup_Pdu_t buffer;
  buffer.command =
      (key->flags & USBQUEUE_RELEASED_MASK) ? SUP_CMD_KEYUP : SUP_CMD_KEYDOWN;
  buffer.data = key->keycode;
  queue_ble_command(&buffer);
  //xprintf("%d %d %d %d", SCQueueReadPos, SCQueueWritePos, SCQueue[SCQueueWritePos].command, SCQueue[SCQueueWritePos].data);
}

void serial_reset_reports() {
  Sup_Pdu_t buffer;
  buffer.command = SUP_CMD_CLEAR;
  buffer.data = 0;
  queue_ble_command(&buffer);
}
