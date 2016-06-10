/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
*/

#pragma once
#include "hidapi.h"
#include "Events.h"
#include <QObject>
#include "LogViewer.h"
#include "../c2/c2_protocol.h"

class DeviceInterface : public QObject
{
    Q_OBJECT
    Q_ENUMS(DeviceStatus)

    public:
        DeviceInterface(QObject *parent = 0);
        ~DeviceInterface();
        void setLogger(LogViewer *l);
        void start(void);
        bool event(QEvent* e);
        device_status_t* getStatus(void);
        LogViewer* logger;
        enum DeviceStatus {DeviceConnected, DeviceDisconnected};

    public slots:
        void sendCommand(uint8_t, QByteArray);
        void sendCommand(uint8_t, uint8_t);

    signals:
        void deviceStatusNotification(DeviceInterface::DeviceStatus);

    protected:
        virtual void timerEvent(QTimerEvent *);

    private:
        hid_device* device;
        int pollTimerId;
        unsigned char outbox[65];
        unsigned char bytesFromDevice[65];
        device_status_t status;
        hid_device* acquireDevice(void);
        void whine(QString msg);
        void resetTimer(int interval);

    private slots:
        void deviceMessageReceiver(void);

};
