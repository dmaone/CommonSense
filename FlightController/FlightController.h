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

namespace Ui {
class FlightController;
}

class FlightController : public QMainWindow
{
    Q_OBJECT

public:
    explicit FlightController(QWidget *parent = 0);
    ~FlightController();

public slots:
    void matrixMonitorButtonClick(void);

private:
    Ui::FlightController *ui;
    MatrixMonitor *mm;

};
