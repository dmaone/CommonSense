/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/
#include <QMessageBox>
#include <QFileDialog>
#include <Qfile>
#include "FlightController.h"
#include "ui_flightcontroller.h"
#include "MatrixMonitor.h"

#include "singleton.h"
#include "DeviceInterface.h"
#include "../c2/c2_protocol.h"
#include "../c2/nvram.h"


FlightController::FlightController(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FlightController), mm(NULL)
{
    for (uint8_t i=0; i<ABSOLUTE_MAX_COLS; i++)
        columns[i] = NULL;
    for (uint8_t i=0; i<ABSOLUTE_MAX_ROWS; i++)
        rows[i] = NULL;
    ui->setupUi(this);
    ui->mainPanel->setCurrentIndex(0);
    connect(ui->ClearButton, SIGNAL(clicked()), ui->LogViewport, SLOT(clearButtonClick()));
    connect(ui->CopyAllButton, SIGNAL(clicked()), ui->LogViewport, SLOT(copyAllButtonClick()));
    connect(ui->RedButton, SIGNAL(toggled(bool)), this, SLOT(redButtonToggle(bool)));
    connect(ui->BootloaderButton, SIGNAL(clicked()), this, SLOT(bootloaderButtonClick()));
    connect(ui->MatrixMonitorButton, SIGNAL(clicked()), this, SLOT(matrixMonitorButtonClick()));
    connect(ui->statusRequestButton, SIGNAL(clicked()), this, SLOT(statusRequestButtonClick()));
    connect(ui->validateButton, SIGNAL(clicked()), this, SLOT(validateConfig()));
    connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(applyConfig()));
    connect(ui->mainPanel, SIGNAL(currentChanged(int)), this, SLOT(mainTabChanged(int)));
    connect(ui->Rows, SIGNAL(valueChanged(int)), this, SLOT(cowsChanged(int)));
    connect(ui->Cols, SIGNAL(valueChanged(int)), this, SLOT(cowsChanged(int)));
    connect(ui->importButton, SIGNAL(clicked()), this, SLOT(importConfig()));
    connect(ui->exportButton, SIGNAL(clicked()), this, SLOT(exportConfig()));

    DeviceInterface &di = Singleton<DeviceInterface>::instance();
    di.setLogger(ui->LogViewport);
    connect(this, SIGNAL(sendCommand(uint8_t,uint8_t)), &di, SLOT(sendCommand(uint8_t, uint8_t)));
    connect(&di, SIGNAL(deviceStatusNotification(DeviceInterface::DeviceStatus)), this, SLOT(deviceStatusNotification(DeviceInterface::DeviceStatus)));
    ui->LogViewport->logMessage("Acquiring device..");
    if (!di.start())
    {
        emit(deviceStatusNotification(DeviceInterface::DeviceDisconnected));
    }
}

void FlightController::closeEvent (QCloseEvent *event)
{
    if (mm)
        mm->close();
    event->accept();
}

FlightController::~FlightController()
{
    if (mm)
        delete mm;
    delete ui;
}
void FlightController::importConfig()
{

}

void FlightController::exportConfig()
{
    QFileDialog fd(this, "Choose one file to export to");
    fd.setNameFilter(tr("CommonSense config files(*.cfg)"));
    fd.setDefaultSuffix(QString("cfg"));
    if (fd.exec())
    {
        QStringList fns = fd.selectedFiles();
        QFile f(fns.at(0));
        f.open(QIODevice::WriteOnly);
        QDataStream ds(&f);
        DeviceInterface &di = Singleton<DeviceInterface>::instance();
        ds.writeRawData((const char *)di.getConfigPtr()->raw, EEPROM_SIZE);
    }
}

QComboBox* FlightController::newMappingCombo(void)
{
    QComboBox *b = new QComboBox();
    b->setInsertPolicy(QComboBox::NoInsert);
    b->setVisible(false);
    connect(b, SIGNAL(currentIndexChanged(int)), this, SLOT(setConfigDirty(int)));
    return b;
}

void FlightController::initSetupDisplay(void)
{
    if (rows[0])
        // Initialized already
        return;
    QGridLayout *lrows = new QGridLayout();
    QGridLayout *lcols = new QGridLayout();
    for (uint8_t i = 0; i<ABSOLUTE_MAX_ROWS; i++)
    {
        columns[i] = newMappingCombo();
        lcols->addWidget(columns[i], 0, i);
        rows[i] = newMappingCombo();
        lrows->addWidget(rows[i], 0, i);
    }
    lcols->setColumnStretch(ABSOLUTE_MAX_ROWS, 1);
    lrows->setColumnStretch(ABSOLUTE_MAX_ROWS, 1);
    for (uint8_t i = ABSOLUTE_MAX_ROWS; i<ABSOLUTE_MAX_COLS; i++)
    {
        columns[i] = newMappingCombo();
        lcols->addWidget(columns[i], 1, i - ABSOLUTE_MAX_ROWS);
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
    this->validateConfig();
}

void FlightController::matrixMonitorButtonClick(void)
{
    if (mm == NULL) {
        mm = new MatrixMonitor(ui->Rows->value(), ui->Cols->value());
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
    if (s == DeviceInterface::DeviceConnected)
    {
        ui->mainPanel->setTabEnabled(1, true);
    }
    else if (s == DeviceInterface::DeviceDisconnected)
    {
        ui->mainPanel->setTabEnabled(1, false);
    }
}

void FlightController::cowsChanged(int idx __attribute__ ((unused)) )
{
    updateSetupDisplay();
    if (mm)
        mm->updateDisplaySize(ui->Rows->value(), ui->Cols->value());
}

void FlightController::setConfigDirty(int)
{
    ui->validateButton->setEnabled(true);
}

FlightController::CowValidationStatus FlightController::validateCow(QComboBox **cows, int cowToCheck, int cowcnt)
{
    bool CowExists = false;
    for (uint8_t i=0; i<cowcnt; i++)
    {
        cows[i]->setStyleSheet(QString(""));
        if (cows[i]->currentIndex() > 0){
            if (cowToCheck == cows[i]->currentIndex())
            {
                if (CowExists) {
                    cows[i]->setStyleSheet(QString("color: White; background-color: #ff0000"));
                    return cvsDuplicate;
                }
                CowExists = true;
            }
        }
    }
    return CowExists ? cvsOK : cvsMissing;
}

QString FlightController::validateCows(QComboBox** cows, int totalCows)
{
    // Account for disabled columns - should be cont. 0 to (total - disabled).
    for (uint8_t i=0; i<totalCows; i++)
    {
        switch (this->validateCow(cows, i+1, totalCows))
        {
        case cvsMissing:
            return QString("Missing: %1").arg(i+1);
            break;
        case cvsDuplicate:
            return QString("Duplicate mapping for row %1").arg(i+1);
        default:
            break;
        }
    }
    return QString();
}

void FlightController::lockTabs(bool dothis)
{
    for (int i=0; i < ui->mainPanel->count(); i++) {
        if (i == ui->mainPanel->currentIndex())
            break;
        ui->mainPanel->setTabEnabled(i, !dothis);
    }
}

bool FlightController::reportValidationFailure(QString msg)
{
    lockTabs(true);
    QMessageBox::critical(this, "Matrix validation failure", msg);
    return false;
}

bool FlightController::matrixMappingValid(void)
{
    QString errmsg;
    errmsg = this->validateCows(rows, ui->Rows->value());
    if (!errmsg.isNull())
        return reportValidationFailure(errmsg);
    errmsg = this->validateCows(columns, ui->Cols->value());
    if (!errmsg.isEmpty())
        return reportValidationFailure(errmsg);

    return true;
}

void FlightController::validateConfig(void)
{
    if (!matrixMappingValid())
        return;
    ui->validateButton->setDisabled(true);
}

void FlightController::applyConfig(void)
{
    std::vector<uint8_t> r;
    std::vector<uint8_t> c;
    DeviceInterface &di = Singleton<DeviceInterface>::instance();
    for(uint8_t i=0; i< ABSOLUTE_MAX_ROWS; i++)
        r.push_back((uint8_t)rows[i]->currentIndex());
    r.push_back(ui->Rows->value());
    for(uint8_t i=0; i< ABSOLUTE_MAX_COLS; i++)
        c.push_back(columns[i]->currentIndex());
    c.push_back(ui->Cols->value());
    di.setMatrixSizeParameters(r, c);
}


void FlightController::mainTabChanged(int idx)
{
    if (idx == 1) {
        initSetupDisplay();
        updateSetupDisplay();
        validateConfig();
    }
}
