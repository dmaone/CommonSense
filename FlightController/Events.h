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

class DeviceMessage: public QEvent {
 public:
    static const QEvent::Type ET;
    DeviceMessage(const unsigned char *buf);
    ~DeviceMessage(void);
    QByteArray* getPayload();

 private:
    QByteArray *payload;
};
