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
#include "DeviceConfig.h"
#include "../c2/c2_protocol.h"
#include "../c2/nvram.h"


FlightController::FlightController(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FlightController), mm(NULL), layoutEditor(NULL), thresholdEditor(NULL)
{
    ui->setupUi(this);
}

void FlightController::setup(void)
{
    DeviceInterface &di = Singleton<DeviceInterface>::instance();

    connect(ui->ClearButton, SIGNAL(clicked()), ui->LogViewport, SLOT(clearButtonClick()));
    connect(ui->CopyAllButton, SIGNAL(clicked()), ui->LogViewport, SLOT(copyAllButtonClick()));
    connect(ui->RedButton, SIGNAL(toggled(bool)), this, SLOT(redButtonToggle(bool)));
    connect(ui->BootloaderButton, SIGNAL(clicked()), this, SLOT(bootloaderButtonClick()));
    connect(ui->MatrixMonitorButton, SIGNAL(clicked()), this, SLOT(matrixMonitorButtonClick()));
    connect(ui->statusRequestButton, SIGNAL(clicked()), this, SLOT(statusRequestButtonClick()));
    connect(ui->layoutButton, SIGNAL(clicked()), this, SLOT(editLayoutClick()));
    connect(ui->thresholdsButton, SIGNAL(clicked()), this, SLOT(editThresholdsClick()));
    connect(ui->mainPanel, SIGNAL(currentChanged(int)), this, SLOT(mainTabChanged(int)));
    connect(ui->importButton, SIGNAL(clicked()), di.config, SLOT(fromFile()));
    connect(ui->uploadButton, SIGNAL(clicked()), di.config, SLOT(toDevice()));
    connect(ui->downloadButton, SIGNAL(clicked()), di.config, SLOT(fromDevice()));
    connect(ui->exportButton, SIGNAL(clicked()), di.config, SLOT(toFile()));
    connect(ui->commitButton, SIGNAL(clicked()), this, SLOT(commitConfig()));
    connect(ui->rollbackButton, SIGNAL(clicked()), this, SLOT(rollbackConfig()));
    connect(this, SIGNAL(sendCommand(c2command, uint8_t)), &di, SLOT(sendCommand(c2command, uint8_t)));
    connect(&di, SIGNAL(deviceStatusNotification(DeviceInterface::DeviceStatus)), this, SLOT(deviceStatusNotification(DeviceInterface::DeviceStatus)));
    di.start();

}

void FlightController::show(void)
{
    ui->mainPanel->setCurrentIndex(0);
    QMainWindow::show();
}

void FlightController::closeEvent (QCloseEvent *event)
{
    if (mm)
        mm->close();
    if (layoutEditor)
        layoutEditor->close();
    event->accept();
}

void FlightController::mainTabChanged(int idx)
{
    if (idx == 1) {
    }
}

FlightController::~FlightController()
{
    delete ui;
}

LogViewer* FlightController::getLogViewport(void)
{
    return ui->LogViewport;
}

void FlightController::logToViewport(const QString &msg)
{
    ui->LogViewport->logMessage(msg);
}


void FlightController::matrixMonitorButtonClick(void)
{
    if (mm == NULL) {
        mm = new MatrixMonitor();
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

void FlightController::deviceStatusNotification(DeviceInterface::DeviceStatus s)
{
    lockTabs();
    switch (s)
    {
        case DeviceInterface::DeviceConnected:
            emit(ui->downloadButton->clicked());
            break;
        case DeviceInterface::DeviceDisconnected:
            ui->mainPanel->setCurrentIndex(0);
            break;
        case DeviceInterface::DeviceConfigChanged:
            unlockTabs();
            ui->mainPanel->setTabEnabled(1, true);
            emit sendCommand(C2CMD_GET_STATUS, 0);
            break;
    }
}

void FlightController::manipulateTabs(bool dothis)
{
    for (int i=0; i < ui->mainPanel->count(); i++) {
        if (i == ui->mainPanel->currentIndex())
            continue;
        ui->mainPanel->setTabEnabled(i, dothis);
    }
}

bool FlightController::reportValidationFailure(QString msg)
{
    lockTabs();
    QMessageBox::critical(this, "Matrix validation failure", msg);
    return false;
}

void FlightController::editLayoutClick(void)
{
    if (layoutEditor == NULL) {
        layoutEditor = new LayoutEditor();
    }
    layoutEditor->show();

}

void FlightController::editThresholdsClick(void)
{
    if (thresholdEditor == NULL) {
        thresholdEditor = new ThresholdEditor();
        connect(thresholdEditor, SIGNAL(logMessage(QString)), ui->LogViewport, SLOT(logMessage(QString)));
    }
    thresholdEditor->show();

}


void FlightController::commitConfig(void)
{
    QMessageBox::StandardButton result = QMessageBox::question(this,
            "Saving EEPROM!",
            "Do you want to write the config that is now in the device, to EEPROM?",
            QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::Yes)
        sendCommand(C2CMD_COMMIT, 1u);
}

void FlightController::rollbackConfig(void)
{
    QMessageBox::StandardButton result = QMessageBox::question(this,
            "Resetting device!",
            "Device will be reset, config will be restored from EEPROM and downloaded to host. OK?",
            QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::Yes)
        sendCommand(C2CMD_ROLLBACK, 1u);
}

