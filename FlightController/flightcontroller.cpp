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
    ui->ColumnMapping->setLayout(new QHBoxLayout());
    for (uint8_t i = 0; i<16; i++)
    {
        QComboBox *b = new QComboBox();
        b->addItem(0, "Skip");
        b->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        b->sizeHint().setWidth(0);
        ui->ColumnMapping->layout()->addWidget(b);
        columns[i] = b;
    }

    ui->RowMapping->setLayout(new QGridLayout());
}

FlightController::~FlightController()
{
    if (mm != NULL)
        delete mm;
    delete ui;
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

void FlightController::layoutMatrixMappings(void)
{
    QHBoxLayout *cl = static_cast<QHBoxLayout *>(ui->ColumnMapping->layout());

}

void FlightController::mainTabChanged(int idx)
{
    if (idx == 1)
        layoutMatrixMappings();
}
