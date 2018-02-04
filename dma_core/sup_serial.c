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

void serial_init(void) {
  SupervisoryUART_Start();
}

void serial_send(Sup_Pdu_t* packet) {
  SupervisoryUART_PutArray(packet->raw, sizeof(packet->raw));
}

bool serial_receive(Sup_Pdu_t* data) {
  if (SupervisoryUART_ReadRxStatus() != SupervisoryUART_RX_STS_FIFO_NOTEMPTY) {
    return false;
  }
  for (uint8_t i=0; i < 3; i++) {
    while (SupervisoryUART_ReadRxStatus()
        != SupervisoryUART_RX_STS_FIFO_NOTEMPTY) {};
    data->raw[i] = SupervisoryUART_ReadRxData();
  }
  return true;
}