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

class DeviceInterface : public QObject
{
    Q_OBJECT

    public:
        DeviceInterface(QObject *parent = 0);
        ~DeviceInterface();
        void setLogger(LogViewer *l);
        void connect(void);
        void listen(void);
        LogViewer* logger;
        bool event(QEvent* e);
    public slots:
        void notifyDevice(QByteArray);

    protected:
        virtual void timerEvent(QTimerEvent *);

    private:
        hid_device* device;
        int pollTimerId;
        unsigned char outbox[65];
        unsigned char bytesFromDevice[65];
        void whine(QString msg);
        void resetTimer(int interval);

    private slots:
        void deviceMessageReceiver(void);

};
