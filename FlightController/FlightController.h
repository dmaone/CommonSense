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
    void show(void);
    enum CowValidationStatus {cvsOK, cvsMissing, cvsDuplicate};
    LogViewer* getLogViewport(void);
    void logToViewport(const QString&);

signals:
    void sendCommand(uint8_t cmd, uint8_t msg);
    void downloadConfig(void);

public slots:
    void redButtonToggle(bool);
    void bootloaderButtonClick(void);
    void matrixMonitorButtonClick(void);
    void statusRequestButtonClick(void);
    void validateConfig(void);
    void editLayoutClick(void);
    void editThresholdsClick(void);
    void applyConfig(void);
    void revertConfig(void);
    void mainTabChanged(int);
    void setConfigDirty(int);
    void cowsChanged(int);
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
    QComboBox *rows[ABSOLUTE_MAX_ROWS];
    QComboBox *columns[ABSOLUTE_MAX_COLS];
    std::vector<uint8_t> row_config;
    std::vector<uint8_t> col_config;
    void initSetupDisplay(void);
    void updateSetupDisplay(void);
    void adjustCows(QComboBox**, int, int);
    bool matrixMappingValid();
    bool reportValidationFailure(QString);
    void manipulateTabs(bool);
    CowValidationStatus validateCow(QComboBox**, int, int);
    QString validateCows(QComboBox**, int);
    QComboBox* newMappingCombo(void);
    //aliases
    void lockTabs() { manipulateTabs(false);}
    void unlockTabs() { manipulateTabs(true);}
};
