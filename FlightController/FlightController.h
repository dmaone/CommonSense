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
#include <QComboBox>
#include "../c2/c2_protocol.h"
#include "DeviceInterface.h"

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

signals:
    void sendCommand(uint8_t cmd, uint8_t msg);


public slots:
    void redButtonToggle(bool);
    void bootloaderButtonClick(void);
    void matrixMonitorButtonClick(void);
    void statusRequestButtonClick(void);
    void importConfig(void);
    void exportConfig(void);
    void validateConfig(void);
    void applyConfig(void);
    void revertConfig(void);
    void mainTabChanged(int);
    void setConfigDirty(int);
    void cowsChanged(int);
    void deviceStatusNotification(DeviceInterface::DeviceStatus);

protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::FlightController *ui;
    MatrixMonitor *mm;
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
