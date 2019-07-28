/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "FlightController.h"
#include "ui_FlightController.h"
#include <QMessageBox>

#include "../c2/c2_protocol.h"
#include "../c2/nvram.h"
#include "DeviceConfig.h"
#include "DeviceInterface.h"
#include "singleton.h"

constexpr size_t kBlinkTimerTick = 100;

FlightController::FlightController(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::FlightController) {
  ui->setupUi(this);
  ui->swVersionLabel->setText(QCoreApplication::applicationVersion());

  DeviceInterface &di = Singleton<DeviceInterface>::instance();

  matrixMonitor = new MatrixMonitor();
  layoutEditor = new LayoutEditor(di.config);
  connect(
      &di,
      SIGNAL(scancodeReceived(uint8_t, uint8_t, DeviceInterface::KeyStatus)),
      layoutEditor,
      SLOT(receiveScancode(uint8_t, uint8_t, DeviceInterface::KeyStatus)));

  thresholdEditor = new ThresholdEditor(di.config);
  connect(
      &di,
      SIGNAL(scancodeReceived(uint8_t, uint8_t, DeviceInterface::KeyStatus)),
      thresholdEditor,
      SLOT(receiveScancode(uint8_t, uint8_t, DeviceInterface::KeyStatus)));

  macroEditor = new MacroEditor(di.config);

  layerConditions = new LayerConditions(di.config);

  _delays = new Delays(di.config);

  _hardware = new Hardware(di.config);

  // Must be last in chain to intercept all packets!
  loader = new FirmwareLoader();
  connect(loader, SIGNAL(switchMode(bool)), &di, SLOT(bootloaderMode(bool)));
  connect(loader, SIGNAL(sendPacket(Bootloader_packet_t *)), &di,
          SLOT(sendCommand(Bootloader_packet_t *)));
  blinkTimerId = startTimer(kBlinkTimerTick);
}

void FlightController::setup(void) {
  DeviceInterface &di = Singleton<DeviceInterface>::instance();

  connect(ui->ClearButton, SIGNAL(clicked()), ui->LogViewport,
          SLOT(clearButtonClick()));
  connect(ui->CopyAllButton, SIGNAL(clicked()), ui->LogViewport,
          SLOT(copyAllButtonClick()));

  connect(ui->actionFirmware_File, SIGNAL(triggered()), loader,
          SLOT(selectFile()));

  connect(ui->MatrixMonitorButton, SIGNAL(clicked()), this,
          SLOT(showKeyMonitor()));
  connect(ui->action_Key_Monitor, SIGNAL(triggered()), this,
          SLOT(showKeyMonitor()));

  connect(ui->thresholdsButton, SIGNAL(clicked()), this,
          SLOT(editThresholdsClick()));
  connect(ui->action_Thresholds, SIGNAL(triggered()), this,
          SLOT(editThresholdsClick()));

  connect(ui->layerModsButton, SIGNAL(clicked()), this,
          SLOT(showLayerConditions()));
  connect(ui->action_Layer_mods, SIGNAL(triggered()), this,
          SLOT(showLayerConditions()));

  connect(ui->layoutButton, SIGNAL(clicked()), this, SLOT(editLayoutClick()));
  connect(ui->action_Layout, SIGNAL(triggered()), this,
          SLOT(editLayoutClick()));

  connect(ui->macrosButton, SIGNAL(clicked()), this, SLOT(editMacrosClick()));
  connect(ui->action_Macros, SIGNAL(triggered()), this,
          SLOT(editMacrosClick()));

  connect(ui->delaysButton, SIGNAL(clicked()), this, SLOT(editDelays()));
  connect(ui->action_Delays, SIGNAL(triggered()), this, SLOT(editDelays()));

  connect(ui->hwButton, SIGNAL(clicked()), this, SLOT(editHardware()));
  connect(ui->action_Hardware, SIGNAL(triggered()), this,
          SLOT(editHardware()));

  connect(ui->BootloaderButton, SIGNAL(clicked()), loader, SLOT(start()));
  connect(ui->action_Update_Firmware, SIGNAL(triggered()), loader,
          SLOT(start()));

  connect(ui->action_Open, SIGNAL(triggered()), di.config, SLOT(fromFile()));
  connect(ui->action_Upload, SIGNAL(triggered()), di.config, SLOT(toDevice()));
  connect(ui->action_Download, SIGNAL(triggered()), di.config,
          SLOT(fromDevice()));
  connect(ui->action_Save, SIGNAL(triggered()), di.config, SLOT(toFile()));
  connect(ui->action_Commit, SIGNAL(triggered()), di.config, SLOT(commit()));
  connect(ui->action_Rollback, SIGNAL(triggered()), di.config,
          SLOT(rollback()));
  connect(this, SIGNAL(sendCommand(c2command, uint8_t)), &di,
          SLOT(sendCommand(c2command, uint8_t)));
  connect(this, SIGNAL(flipStatusBit(deviceStatus)), &di,
          SLOT(flipStatusBit(deviceStatus)));
  connect(this, SIGNAL(setStatusBit(deviceStatus, bool)), &di,
          SLOT(setStatusBit(deviceStatus, bool)));
  connect(&di, SIGNAL(deviceStatusNotification(DeviceInterface::DeviceStatus)),
          this, SLOT(deviceStatusNotification(DeviceInterface::DeviceStatus)));
  lockUI(true);
  di.start();
}

void FlightController::blinkLights() {
  DeviceInterface &di = Singleton<DeviceInterface>::instance();
  if (di.tx) {
    ui->txLabel
        ->setStyleSheet("color: #000000; background-color: #00ff00");
  } else {
    ui->txLabel
        ->setStyleSheet("color: #000000; background-color: #dddddd");
  }
  di.tx = false;
  if (di.rx) {
    ui->rxLabel
        ->setStyleSheet("color: #000000; background-color: #00ff00");
  } else {
    ui->rxLabel
        ->setStyleSheet("color: #000000; background-color: #dddddd");
  }
  di.rx = false;
}

void FlightController::timerEvent(QTimerEvent * timer) {
  if (timer->timerId() == blinkTimerId) {
    blinkLights();
  }
}

void FlightController::updateStatus(void) {
  DeviceInterface &di = Singleton<DeviceInterface>::instance();
  ui->fwVersionLabel->setText(di.firmwareVersion);
  ui->tempGauge->setText(di.dieTemp);
  if (di.scanEnabled) {
    ui->scanButton
        ->setStyleSheet("color: #000000; background-color: #00ff00");
  } else {
    ui->scanButton
        ->setStyleSheet("color: #999999; background-color: #cccccc");
  }
  if (di.outputEnabled) {
    ui->outputButton
        ->setStyleSheet("color: #000000; background-color: #00ff00");
  } else {
    ui->outputButton
        ->setStyleSheet("color: #999999; background-color: #cccccc");
  }
  ui->action_Setup_mode->setChecked(di.setupMode);
  if (di.setupMode) {
    ui->setupButton
        ->setStyleSheet("color: #000000; background-color: #ffff00");
  } else {
      ui->setupButton
        ->setStyleSheet("color: #999999; background-color: #cccccc");
  }
  if (di.matrixMonitor) {
    ui->monitorLabel
        ->setStyleSheet("color: #000000; background-color: #ffff00");
  } else {
      ui->monitorLabel
        ->setStyleSheet("color: #999999; background-color: #cccccc");
  }
  if (di.controllerInsane) {
    ui->insaneLabel
        ->setStyleSheet("color: #000000; background-color: #ff0000");
  } else {
      ui->insaneLabel
        ->setStyleSheet("color: #999999; background-color: #cccccc");
  }
}

void FlightController::show(void) {
  QMainWindow::show();
}

void FlightController::closeEvent(QCloseEvent *event) {
  qInstallMessageHandler(*_oldLogger);
  DeviceInterface &di = Singleton<DeviceInterface>::instance();
  di.sendCommandNow(C2CMD_SET_MODE, C2DEVMODE_NORMAL);
  QApplication::quit();
  event->accept();
}

FlightController::~FlightController() {
  qInstallMessageHandler(*_oldLogger);
  delete ui;
}

LogViewer *FlightController::getLogViewport(void) { return ui->LogViewport; }

void FlightController::setOldLogger(QtMessageHandler *logger) {
  _oldLogger = logger;
}

void FlightController::logToViewport(const QString &msg) {
  ui->LogViewport->logMessage(msg);
}

void FlightController::showKeyMonitor(void) { matrixMonitor->show(); }

void FlightController::deviceStatusNotification(
    DeviceInterface::DeviceStatus s) {
  switch (s) {
  case DeviceInterface::DeviceConnected:
    lockUI(true);
    emit sendCommand(C2CMD_SET_MODE, C2DEVMODE_SETUP);
    emit ui->action_Download->triggered();
    break;
  case DeviceInterface::DeviceDisconnected:
    lockUI(true);
    break;
  case DeviceInterface::DeviceConfigChanged:
    layerConditions->init();
    _delays->init();
    _hardware->init();
    lockUI(false);
    break;
  case DeviceInterface::BootloaderConnected:
    lockUI(true);
    loader->load();
    break;
  case DeviceInterface::StatusUpdated:
    updateStatus();
    break;
  default:
    qCritical() << "Unknown device status" << s << "!";
  }
}

void FlightController::lockUI(bool lock) {
  _uiLocked = lock;
  ui->statusRequestButton->setDisabled(lock);
  ui->MatrixMonitorButton->setDisabled(lock);
  ui->thresholdsButton->setDisabled(lock);
  ui->macrosButton->setDisabled(lock);
  ui->layoutButton->setDisabled(lock);
  ui->layerModsButton->setDisabled(lock);
  ui->delaysButton->setDisabled(lock);
  ui->hwButton->setDisabled(lock);
}

void FlightController::editLayoutClick(void) {
  layoutEditor->show();
  layoutEditor->raise();
}

void FlightController::editMacrosClick(void) { macroEditor->show(); }

void FlightController::editThresholdsClick(void) { thresholdEditor->show(); }

void FlightController::showLayerConditions(void) {
  layerConditions->show();
  layerConditions->raise();
}

void FlightController::on_action_Setup_mode_triggered(bool bMode) {
  emit sendCommand(C2CMD_SET_MODE, bMode ? C2DEVMODE_SETUP : C2DEVMODE_NORMAL);
}

void FlightController::editDelays() {
  _delays->show();
  _delays->raise();
}

void FlightController::editHardware() {
  _hardware->show();
  _hardware->raise();
}

void FlightController::on_scanButton_clicked() {
  emit flipStatusBit(C2DEVSTATUS_SCAN_ENABLED);
}

void FlightController::on_outputButton_clicked() {
  emit flipStatusBit(C2DEVSTATUS_OUTPUT_ENABLED);
}

void FlightController::on_setupButton_clicked() {
  emit flipStatusBit(C2DEVSTATUS_SETUP_MODE);
}

void FlightController::on_redButton_clicked() {
  ui->redButton->setStyleSheet("color: #ff0000; background-color: #990000");
  emit sendCommand(C2CMD_EWO, 0);
}

void FlightController::on_reconnectButton_clicked() {
  qInfo() << "reconnecting..";
  emit setStatusBit(C2DEVSTATUS_SETUP_MODE, false);
  DeviceInterface &di = Singleton<DeviceInterface>::instance();
  di.releaseDevice();
}

void FlightController::on_statusRequestButton_clicked(void) {
  DeviceInterface &di = Singleton<DeviceInterface>::instance();
  di.printableStatus = true;
}
