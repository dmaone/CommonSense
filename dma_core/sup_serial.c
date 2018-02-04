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

Sup_Pdu_t inbox;
uint8_t current_inbox_position = 0;

void serial_init(void) {
  SupervisoryUART_Start();
}

void serial_send(Sup_Pdu_t* packet) {
  SupervisoryUART_PutArray(packet->raw, sizeof(packet->raw));
}

bool serial_receive(Sup_Pdu_t* data) {
  while
      (SupervisoryUART_ReadRxStatus() == SupervisoryUART_RX_STS_FIFO_NOTEMPTY) {
    inbox.raw[current_inbox_position++] = SupervisoryUART_ReadRxData();
    if (current_inbox_position >= sizeof(inbox)) {
      memcpy(data, &inbox, sizeof(inbox));
      current_inbox_position = 0;
      return true;
    }
  }
  return false;
}

void serial_tick(void) {
  Sup_Pdu_t data;
  if (serial_receive(&data)) {
    serial_send(&data);
  }
}