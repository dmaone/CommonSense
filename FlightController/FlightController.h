/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/
#pragma once

#include <QMainWindow>
#include <MatrixMonitor.h>
#include <DeviceInterface.h>
#include <LayoutEditor.h>
#include <ThresholdEditor.h>
#include <QComboBox>
#include "../c2/c2_protocol.h"

namespace Ui {
class FlightController;
}

class FlightController : public QMainWindow
{
    Q_OBJECT
    Q_ENUMS(CowValidationStatus)

public:
    explicit FlightController(QWidget *parent = 0);
    ~FlightController();
    void setup(void);
    void show(void);
    LogViewer* getLogViewport(void);
    void logToViewport(const QString&);

signals:
    void sendCommand(c2command cmd, uint8_t msg);

public slots:
    void redButtonToggle(bool);
    void bootloaderButtonClick(void);
    void matrixMonitorButtonClick(void);
    void statusRequestButtonClick(void);
    void editLayoutClick(void);
    void editThresholdsClick(void);
    void mainTabChanged(int);
    void deviceStatusNotification(DeviceInterface::DeviceStatus);
    void commitConfig(void);
    void rollbackConfig(void);

protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::FlightController *ui;
    MatrixMonitor *mm;
    LayoutEditor *layoutEditor;
    ThresholdEditor *thresholdEditor;
    bool reportValidationFailure(QString);
    void manipulateTabs(bool);
    //aliases
    void lockTabs() { manipulateTabs(false);}
    void unlockTabs() { manipulateTabs(true);}
};
