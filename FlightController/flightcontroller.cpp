/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
*/
#include <QMessageBox>
#include "FlightController.h"
#include "ui_flightcontroller.h"
#include "MatrixMonitor.h"

#include "singleton.h"
#include "DeviceInterface.h"
#include "../c2/c2_protocol.h"


FlightController::FlightController(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FlightController), mm(NULL)
{
    ui->setupUi(this);
    ui->mainPanel->setCurrentIndex(0);
    connect(ui->ClearButton, SIGNAL(clicked()), ui->LogViewport, SLOT(clearButtonClick()));
    connect(ui->CopyAllButton, SIGNAL(clicked()), ui->LogViewport, SLOT(copyAllButtonClick()));
    connect(ui->RedButton, SIGNAL(toggled(bool)), this, SLOT(redButtonToggle(bool)));
    connect(ui->BootloaderButton, SIGNAL(clicked()), this, SLOT(bootloaderButtonClick()));
    connect(ui->MatrixMonitorButton, SIGNAL(clicked()), this, SLOT(matrixMonitorButtonClick()));
    connect(ui->statusRequestButton, SIGNAL(clicked()), this, SLOT(statusRequestButtonClick()));
    DeviceInterface &di = Singleton<DeviceInterface>::instance();
    di.setLogger(ui->LogViewport);
    di.start();
    connect(this, SIGNAL(sendCommand(uint8_t,uint8_t)), &di, SLOT(sendCommand(uint8_t, uint8_t)));
    connect(ui->mainPanel, SIGNAL(currentChanged(int)), this, SLOT(mainTabChanged(int)));
}

FlightController::~FlightController()
{
    if (mm != NULL)
        delete mm;
    delete ui;
}

void FlightController::initSetupDisplay(void)
{
    QHBoxLayout *lrows = new QHBoxLayout();
    QGridLayout *lcols = new QGridLayout();
    for (uint8_t i = 0; i<ABSOLUTE_MAX_ROWS; i++)
    {
        QComboBox *b = new QComboBox();
        b->setInsertPolicy(QComboBox::NoInsert);
        b->setVisible(false);
        lcols->addWidget(b, 0, i);
        columns[i] = b;
        b = new QComboBox();
        b->setInsertPolicy(QComboBox::NoInsert);
        b->setVisible(false);
        lrows->addWidget(b);
        rows[i] = b;
    }
    lrows->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
    for (uint8_t i = 0; i<(ABSOLUTE_MAX_COLS - ABSOLUTE_MAX_ROWS); i++)
    {
        QComboBox *b = new QComboBox();
        b->setInsertPolicy(QComboBox::NoInsert);
        b->setVisible(false);
        lcols->addWidget(b, 1, i);
        columns[i+16] = b;
    }
    ui->ColumnMapping->setLayout(lcols);
    ui->RowMapping->setLayout(lrows);
}
void FlightController::adjustCows(QComboBox *target[], int max, int cnt)
{
    QStringList items;
    items.append("--");
    for (uint8_t i = 0; i < cnt; i++)
        items.append(QString("%1").arg(i+1));

    for (uint8_t i = 0; i<max; i++)
    {
        target[i]->setVisible(false);
        target[i]->clear();
        if (i < cnt) {
            target[i]->addItems(items);
            target[i]->setCurrentIndex(i+1);
            target[i]->setSizeAdjustPolicy(QComboBox::AdjustToContents);
            target[i]->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
            target[i]->sizeHint().setWidth(0);
            target[i]->setVisible(true);
        }
    }
}

void FlightController::updateSetupDisplay(void)
{
    adjustCows(rows, ABSOLUTE_MAX_ROWS, ui->Rows->value());
    adjustCows(columns, ABSOLUTE_MAX_COLS, ui->Cols->value());
}

void FlightController::matrixMonitorButtonClick(void)
{
    if (mm == NULL) {
        mm = new MatrixMonitor(16, 8);
    }
    mm->show();
}

void FlightController::redButtonToggle(bool state)
{
    emit sendCommand(C2CMD_EWO, state);
}

void FlightController::bootloaderButtonClick(void)
{
    QMessageBox::StandardButton result = QMessageBox::question(this,
            "Are you sure?",
            "You will lose communication with the device until reset or firmware update!",
            QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::Yes)
        emit sendCommand(C2CMD_ENTER_BOOTLOADER, 1);
}

void FlightController::statusRequestButtonClick(void)
{
    emit sendCommand(C2CMD_GET_STATUS, 0);
}

void FlightController::mainTabChanged(int idx)
{
    if (idx == 1) {
        initSetupDisplay();
        updateSetupDisplay();
    }
}
