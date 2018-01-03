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

FlightController::FlightController(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::FlightController) {
  ui->setupUi(this);

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

  layerConditions = new LayerConditions(di.config);

  _delays = new Delays(di.config);

  _expHeader = new ExpansionHeader(di.config);

  // Must be last in chain to intercept all packets!
  loader = new FirmwareLoader();
  connect(loader, SIGNAL(switchMode(bool)), &di, SLOT(bootloaderMode(bool)));
  connect(loader, SIGNAL(sendPacket(Bootloader_packet_t *)), &di,
          SLOT(sendCommand(Bootloader_packet_t *)));
}

void FlightController::setup(void) {
  DeviceInterface &di = Singleton<DeviceInterface>::instance();

  connect(ui->ClearButton, SIGNAL(clicked()), ui->LogViewport,
          SLOT(clearButtonClick()));
  connect(ui->CopyAllButton, SIGNAL(clicked()), ui->LogViewport,
          SLOT(copyAllButtonClick()));
  connect(ui->RedButton, SIGNAL(toggled(bool)), this,
          SLOT(redButtonToggle(bool)));

  connect(ui->actionFirmware_File, SIGNAL(triggered()), loader,
          SLOT(selectFile()));

  connect(ui->statusRequestButton, SIGNAL(clicked()), this,
          SLOT(statusRequestButtonClick()));

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

  connect(ui->expButton, SIGNAL(clicked()), this, SLOT(editExpHeader()));
  connect(ui->action_Exp_Header, SIGNAL(triggered()), this,
          SLOT(editExpHeader()));

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
  connect(&di, SIGNAL(deviceStatusNotification(DeviceInterface::DeviceStatus)),
          this, SLOT(deviceStatusNotification(DeviceInterface::DeviceStatus)));
  lockUI(true);
  _statusDisplay[StatusPosition::Version] = new QLabel(" v-.- ");
  _statusDisplay[StatusPosition::Scan] = new QLabel(" scan ");
  _statusDisplay[StatusPosition::Output] = new QLabel(" kbd ");
  _statusDisplay[StatusPosition::Setup] = new QLabel(" setup ");
  _statusDisplay[StatusPosition::Monitor] = new QLabel(" mon ");
  _statusDisplay[StatusPosition::Insane] = new QLabel(" !!! ");
  for (size_t i=0; i < StatusPositionMax; i++) {
    ui->statusBar->addPermanentWidget(_statusDisplay[i]);
  }
  di.start();
  di.installEventFilter(this);
  startTimer(1000);
}

void FlightController::timerEvent(QTimerEvent *) {
  if (!_uiLocked) {
    emit sendCommand(C2CMD_GET_STATUS, 1);
  }
}

bool FlightController::eventFilter(QObject *obj __attribute__((unused)),
                                QEvent *event) {
  if (event->type() == DeviceMessage::ET) {
    QByteArray *pl = static_cast<DeviceMessage *>(event)->getPayload();
    if (pl->at(0) != C2RESPONSE_STATUS) {
      return false;
    }
    _statusDisplay[StatusPosition::Version]
        ->setText(
          QString("v%1.%2").arg((uint8_t)pl->at(2)).arg((uint8_t)pl->at(3)));
    if (pl->at(1) & (1 << C2DEVSTATUS_SCAN_ENABLED)) {
      _statusDisplay[StatusPosition::Scan]
          ->setStyleSheet("color: #000000; background-color: #00ff00");
    } else {
      _statusDisplay[StatusPosition::Scan]
          ->setStyleSheet("color: #999999; background-color: #cccccc");
    }
    if (pl->at(1) & (1 << C2DEVSTATUS_OUTPUT_ENABLED)) {
      _statusDisplay[StatusPosition::Output]
          ->setStyleSheet("color: #000000; background-color: #00ff00");
    } else {
      _statusDisplay[StatusPosition::Output]
          ->setStyleSheet("color: #999999; background-color: #cccccc");
    }
    if (pl->at(1) & (1 << C2DEVSTATUS_SETUP_MODE)) {
      _statusDisplay[StatusPosition::Setup]
          ->setStyleSheet("color: #000000; background-color: #ffff00");
    } else {
      _statusDisplay[StatusPosition::Setup]
          ->setStyleSheet("color: #999999; background-color: #cccccc");
    }
    if (pl->at(1) & (1 << C2DEVSTATUS_MATRIX_MONITOR)) {
      _statusDisplay[StatusPosition::Monitor]
          ->setStyleSheet("color: #000000; background-color: #ffff00");
    } else {
      _statusDisplay[StatusPosition::Monitor]
          ->setStyleSheet("color: #999999; background-color: #cccccc");
    }
    if (pl->at(1) & (1 << C2DEVSTATUS_INSANE)) {
      _statusDisplay[StatusPosition::Insane]
          ->setStyleSheet("color: #000000; background-color: #ff0000");
    } else {
      _statusDisplay[StatusPosition::Insane]
          ->setStyleSheet("color: #999999; background-color: #cccccc");
    }
    return true;
  }
  return false;
}

void FlightController::show(void) { QMainWindow::show(); }

void FlightController::closeEvent(QCloseEvent *event) {
  emit sendCommand(C2CMD_SET_MODE, C2DEVMODE_NORMAL);
  QApplication::quit();
  event->accept();
}

void FlightController::mainTabChanged(int idx) {
  if (idx == 1) {
  }
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

void FlightController::redButtonToggle(bool state) {
  emit sendCommand(C2CMD_EWO, state);
}

void FlightController::statusRequestButtonClick(void) {
  emit sendCommand(C2CMD_GET_STATUS, 0);
}

void FlightController::deviceStatusNotification(
    DeviceInterface::DeviceStatus s) {
  lockUI(true);
  switch (s) {
  case DeviceInterface::DeviceConnected:
    emit ui->action_Download->triggered();
    break;
  case DeviceInterface::DeviceDisconnected:
    break;
  case DeviceInterface::DeviceConfigChanged:
    emit sendCommand(C2CMD_SET_MODE, C2DEVMODE_SETUP);
    ui->action_Setup_mode->setChecked(true);
    layerConditions->init();
    _delays->init();
    _expHeader->init();
    lockUI(false);
    break;
  case DeviceInterface::BootloaderConnected:
    loader->load();
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
  ui->layoutButton->setDisabled(lock);
  ui->layerModsButton->setDisabled(lock);
  ui->delaysButton->setDisabled(lock);
  ui->expButton->setDisabled(lock);
}

void FlightController::editLayoutClick(void) { layoutEditor->show(); }

void FlightController::editMacrosClick(void) {
  // macrosEditor->show();
}

void FlightController::editThresholdsClick(void) { thresholdEditor->show(); }

void FlightController::showLayerConditions(void) { layerConditions->show(); }

void FlightController::on_action_Setup_mode_triggered(bool bMode) {
  emit sendCommand(C2CMD_SET_MODE, bMode ? C2DEVMODE_SETUP : C2DEVMODE_NORMAL);
}

void FlightController::editDelays() { _delays->show(); }

void FlightController::editExpHeader() { _expHeader->show(); }
