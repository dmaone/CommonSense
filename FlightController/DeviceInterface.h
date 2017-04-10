/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#pragma once
#include "hidapi/hidapi.h"
#include "Events.h"
#include <QObject>
#include "LogViewer.h"
#include "DeviceConfig.h"
#include "../c2/c2_protocol.h"

class DeviceInterface : public QObject
{
    Q_OBJECT
    Q_ENUMS(DeviceStatus)
    Q_ENUMS(KeyStatus)
    Q_ENUMS(Mode)

    public:
        DeviceInterface(QObject *parent = 0);
        ~DeviceInterface();
        void start(void);
        bool event(QEvent* e);
        device_status_t* getStatus(void);
        DeviceConfig* config;
        enum DeviceStatus {DeviceConnected, DeviceDisconnected, DeviceConfigChanged, BootloaderConnected};
        enum KeyStatus {KeyPressed, KeyReleased};
        enum Mode {DeviceInterfaceNormal, DeviceInterfaceBootloader};

    public slots:
        void sendCommand(c2command, uint8_t*);
        void sendCommand(c2command, uint8_t);
        void sendCommand(OUT_c2packet_t);
        void sendCommand(Bootloader_packet_t *);
        void configChanged(void);
        void bootloaderMode(bool bEnable);

    signals:
        void deviceStatusNotification(DeviceInterface::DeviceStatus);
        void scancodeReceived(uint8_t row, uint8_t col, DeviceInterface::KeyStatus status);

    protected:
        virtual void timerEvent(QTimerEvent *);

    private:
        hid_device* device;
        int pollTimerId;
        unsigned char outbox[65];
        unsigned char bytesFromDevice[65];
        device_status_t status;
        uint8_t mode;
        DeviceStatus currentStatus;

        hid_device* acquireDevice(void);
        void _releaseDevice(void);
        void _initDevice(void);
        void _resetTimer(int interval);
        void _sendPacket(void);
        void _updateDeviceStatus(DeviceStatus);

    private slots:
        void deviceMessageReceiver(void);

};
