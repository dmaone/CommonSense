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
    initSetupDisplay();

    connect(ui->ClearButton, SIGNAL(clicked()), ui->LogViewport, SLOT(clearButtonClick()));
    connect(ui->CopyAllButton, SIGNAL(clicked()), ui->LogViewport, SLOT(copyAllButtonClick()));
    connect(ui->RedButton, SIGNAL(toggled(bool)), this, SLOT(redButtonToggle(bool)));
    connect(ui->BootloaderButton, SIGNAL(clicked()), this, SLOT(bootloaderButtonClick()));
    connect(ui->MatrixMonitorButton, SIGNAL(clicked()), this, SLOT(matrixMonitorButtonClick()));
    connect(ui->statusRequestButton, SIGNAL(clicked()), this, SLOT(statusRequestButtonClick()));
    connect(ui->validateButton, SIGNAL(clicked()), this, SLOT(validateConfig()));
    connect(ui->layoutButton, SIGNAL(clicked()), this, SLOT(editLayoutClick()));
    connect(ui->thresholdsButton, SIGNAL(clicked()), this, SLOT(editThresholdsClick()));
    connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(applyConfig()));
    connect(ui->mainPanel, SIGNAL(currentChanged(int)), this, SLOT(mainTabChanged(int)));
    connect(ui->Rows, SIGNAL(valueChanged(int)), this, SLOT(cowsChanged(int)));
    connect(ui->Cols, SIGNAL(valueChanged(int)), this, SLOT(cowsChanged(int)));
    connect(ui->importButton, SIGNAL(clicked()), di.config, SLOT(fromFile()));
    connect(ui->uploadButton, SIGNAL(clicked()), di.config, SLOT(toDevice()));
    connect(ui->downloadButton, SIGNAL(clicked()), di.config, SLOT(fromDevice()));
    connect(ui->exportButton, SIGNAL(clicked()), di.config, SLOT(toFile()));
    connect(ui->commitButton, SIGNAL(clicked()), this, SLOT(commitConfig()));
    connect(ui->rollbackButton, SIGNAL(clicked()), this, SLOT(rollbackConfig()));
    connect(this, SIGNAL(sendCommand(uint8_t, uint8_t)), &di, SLOT(sendCommand(uint8_t, uint8_t)));
    connect(&di, SIGNAL(deviceStatusNotification(DeviceInterface::DeviceStatus)), this, SLOT(deviceStatusNotification(DeviceInterface::DeviceStatus)));
    di.start();

}

void FlightController::show(void)
{
    ui->mainPanel->setCurrentIndex(0);
    QMainWindow::show();
    //DeviceInterface &di = Singleton<DeviceInterface>::instance();
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
        initSetupDisplay();
        updateSetupDisplay();
        validateConfig();
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

void FlightController::revertConfig()
{
    /*TODO
    DeviceInterface &di = Singleton<DeviceInterface>::instance();
    psoc_eeprom_t* config = di.getConfigPtr();
    ui->Rows->setValue(config->matrixRows);
    ui->Cols->setValue(config->matrixCols);
    ui->normallyOpen->setChecked(config->capsenseFlags & (1 << CSF_NL));
    */
    updateSetupDisplay();
    qInfo() << "GUI synced with config";
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
    if (ui->ColumnMapping->isEnabled())
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
    ui->ColumnMapping->setEnabled(true);
    ui->RowMapping->setLayout(lrows);
    ui->RowMapping->setEnabled(true);
}

void FlightController::adjustCows(QComboBox *target[], int max, int cnt)
{
    QStringList items;
    items.append("--");
    for (uint8_t i = 0; i < cnt; i++)
        items.append(QString("%1").arg(i+1));
    for (uint8_t i = 0; i<max; i++)
    {
        uint8_t idx = target[i]->currentIndex();
        target[i]->setVisible(false);
        target[i]->clear();
        if (i < cnt) {
            target[i]->addItems(items);
            target[i]->setCurrentIndex(idx);
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
            revertConfig();
            unlockTabs();
            ui->mainPanel->setTabEnabled(1, true);
            emit sendCommand(C2CMD_GET_STATUS, 0);
            break;
    }
}

void FlightController::cowsChanged(int idx __attribute__ ((unused)) )
{
    updateSetupDisplay();
    if (mm)
        delete mm;
    mm = NULL;
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

void FlightController::editLayoutClick(void)
{
    if (layoutEditor == NULL) {
        layoutEditor = new LayoutEditor();
        connect(layoutEditor, SIGNAL(logMessage(QString)), ui->LogViewport, SLOT(logMessage(QString)));
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


void FlightController::validateConfig(void)
{
    if (ui->mainPanel->currentIndex() == 1)
    {
        if (!matrixMappingValid())
            return;
/*  FIXME This gets too boring. Better validations after the keyboard works.
    DeviceInterface &di = Singleton<DeviceInterface>::instance();
    psoc_eeprom_t* config = di.getConfigPtr();
    if (ui->Rows->value() < config->matrixRows())
    Also do not forget to really reset all row/column-dependent stuff.
*/
    }
    ui->validateButton->setDisabled(true);
    unlockTabs();
}

void FlightController::applyConfig(void)
{
    //TODO DeviceInterface &di = Singleton<DeviceInterface>::instance();
    // TODO psoc_eeprom_t* config = di.getConfigPtr();
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

