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

namespace Ui {
class FlightController;
}

class FlightController : public QMainWindow
{
    Q_OBJECT

public:
    explicit FlightController(QWidget *parent = 0);
    ~FlightController();

signals:
    void sendCommand(uint8_t cmd, uint8_t msg);

public slots:
    void redButtonToggle(bool);
    void bootloaderButtonClick(void);
    void matrixMonitorButtonClick(void);
    void statusRequestButtonClick(void);
    void mainTabChanged(int);

private:
    Ui::FlightController *ui;
    MatrixMonitor *mm;
    QComboBox *rows[ABSOLUTE_MAX_ROWS];
    QComboBox *columns[ABSOLUTE_MAX_COLS];
    void initSetupDisplay(void);
    void updateSetupDisplay(void);
    void adjustCows(QComboBox**, int, int);
};
