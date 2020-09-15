/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#pragma once

#include <QtCore>

class DeviceMessage : public QEvent {
 public:
  explicit DeviceMessage(const unsigned char *buf) :
      QEvent(DeviceMessage::ET), payload((const char *)buf, 64) {};

  QByteArray* getPayload() {
    return &payload;
  }

  static const QEvent::Type ET;
  QByteArray payload;
};
