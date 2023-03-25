/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "FlightController.h"

#include "../c2/nvram.h"
#include "DeviceConfig.h"
#include "DeviceInterface.h"

constexpr size_t kBlinkTimerTick = 20;

namespace {
constexpr auto kRed{"color: #000000; background-color: #ff0000"};
constexpr auto kGreen{"color: #000000; background-color: #00ff00"};
constexpr auto kYellow{"color: #000000; background-color: #ffff00"};

constexpr auto kOff{"color: #000000; background-color: #dddddd"};
constexpr auto kDisabled{"color: #999999; background-color: #dddddd"};

static LogViewer* logger{nullptr};

void logToViewport(
    QtMsgType /* type */,
    const QMessageLogContext& /* ctx */,
    const QString &msg) {
  if (msg.length() < 3) {
    logger->continueMessage(msg);
  } else {
    logger->logMessage(msg);
  }
}

} // namespace

FlightController::FlightController(bool useCustomMessageHandler)
  : delays_{di_.config},
    hardware_{di_.config},
    layers_{di_.config},
    layout_{di_},
    loader_{di_},
    macros_{di_.config},
    pedals_{di_},
    telemetry_{di_},
    thresholds_{di_} {
  ui->setupUi(this);
  lockUI_(true);
  connectBackendSlots_();
  connectUiSlots_();
  ui->swVersionLabel->setText(QCoreApplication::applicationVersion());
  logger = ui->LogViewport;

  if (useCustomMessageHandler) {
    qInstallMessageHandler(&logToViewport);
  }

  blinkTimerId_ = startTimer(kBlinkTimerTick);
  di_.start();
}

FlightController::~FlightController() {
  qInstallMessageHandler(nullptr);
}

void FlightController::connectBackendSlots_() {
  // Must be last in chain to intercept all packets!
  // This means that all connect() calls elsewhere must be in ctors ONLY.
  connect(this, SIGNAL(sendCommand(c2command, uint8_t)), &di_,
          SLOT(sendCommand(c2command, uint8_t)));
  connect(this, SIGNAL(flipStatusBit(deviceStatus)), &di_,
          SLOT(flipStatusBit(deviceStatus)));
  connect(this, SIGNAL(setStatusBit(deviceStatus, bool)), &di_,
          SLOT(setStatusBit(deviceStatus, bool)));
  connect(&di_, SIGNAL(notify(DeviceInterface::State)),
          this, SLOT(deviceInterfaceNotification(DeviceInterface::State)));
}

/*
 * The idea: everything is attached to _actions_. Buttons trigger actions.
 */
void FlightController::connectUiSlots_() {
  auto click = [this] (auto button, auto who, auto slot) {
    this->connect(button, SIGNAL(clicked()), who, slot);
  };

  auto trig = [this] (auto action, auto& who, auto slot) {
    this->connect(action, SIGNAL(triggered()), &who, slot);
  };
  auto trigThis = [this] (auto action, auto slot) {
    this->connect(action, SIGNAL(triggered()), this, slot);
  };

  auto click2trig = [this](auto button, auto& action) {
    this->connect(button, SIGNAL(clicked()), action, SLOT(trigger()));
  };

  // File menu
  trig(ui->action_Download, di_.config, SLOT(fromDevice()));

  trig(ui->action_Open, di_.config, SLOT(fromFile()));
  trig(ui->action_Save, di_.config, SLOT(toFile()));
  trig(ui->action_Upload, di_.config, SLOT(toDevice()));

  // Window menu
  trigThis(ui->action_Delays, SLOT(showDelays_()));
  trigThis(ui->action_Hardware, SLOT(showHardware_()));
  trigThis(ui->action_Telemetry, SLOT(showTelemetry_()));
  trigThis(ui->action_Layer_mods, SLOT(showLayers_()));
  trigThis(ui->action_Layout, SLOT(showLayout_()));
  trigThis(ui->action_Macros, SLOT(showMacros_()));
  trigThis(ui->action_Pedals, SLOT(showPedals_()));
  trigThis(ui->action_Thresholds, SLOT(showThresholds_()));

  // Action menu
  trig(ui->actionFirmware_File, loader_, SLOT(selectFile()));
  trig(ui->action_Update_Firmware, loader_, SLOT(start()));

  trig(ui->action_Commit, di_.config, SLOT(commit()));
  trig(ui->action_Rollback, di_.config, SLOT(rollback()));

  connect(
      ui->action_Setup_mode, SIGNAL(toggled(bool)), this, SLOT(setMode_(bool)));

  // Button mapping to actions
  click2trig(ui->BootloaderButton, ui->action_Update_Firmware);
  click2trig(ui->delaysButton, ui->action_Delays);
  click2trig(ui->hwButton, ui->action_Hardware);
  click2trig(ui->TelemetryButton, ui->action_Telemetry);
  click2trig(ui->layerModsButton, ui->action_Layer_mods);
  click2trig(ui->layoutButton, ui->action_Layout);
  click2trig(ui->macrosButton, ui->action_Macros);
  click2trig(ui->pedalsButton, ui->action_Pedals);
  click2trig(ui->thresholdsButton, ui->action_Thresholds);

  // Buttons without actions
  click(ui->ClearButton, ui->LogViewport, SLOT(clearAll()));
  click(ui->CopyAllButton, ui->LogViewport, SLOT(copyAll()));
  click(ui->reconnectButton, this, SLOT(resetConnection_()));
  click(ui->outputButton, this, SLOT(toggleOutput_()));
  click(ui->scanButton, this, SLOT(toggleScan_()));
  click(ui->setupButton, this, SLOT(toggleSetupMode_()));
}

void FlightController::deviceInterfaceNotification(DeviceInterface::State s) {
  switch (s) {
  case DeviceInterface::DeviceConnected:
    lockUI_(true);
    emit sendCommand(C2CMD_SET_MODE, C2DEVMODE_SETUP);
    emit ui->action_Download->triggered();
    break;
  case DeviceInterface::DeviceDisconnected:
    lockUI_(true);
    telemetry_.deinit();
    break;
  case DeviceInterface::DeviceConfigured:
    delays_.init();
    hardware_.init();
    layers_.init();
    layout_.init();
    pedals_.init();
    telemetry_.init();
    thresholds_.init();
    ui->typeLabel->setText(di_.config.getSwitchTypeName());
    lockUI_(false);
    break;
  case DeviceInterface::ThresholdsUpdated:
      // Partial update of UI controls to update thresholds info
      thresholds_.init();
      layout_.init();
      pedals_.init();
      break;
  case DeviceInterface::BootloaderConnected:
    lockUI_(true);
    loader_.load();
    break;
  case DeviceInterface::StatusUpdated:
    ui->fwVersionLabel->setText(di_.firmwareVersion);
    ui->tempGauge->setText(di_.dieTemp);
    ui->sysTimeLabel->setText(QString("%1").arg(di_.deviceTime));
    ui->sysClockDriftLabel->setText(QString("%1").arg(di_.deviceDrift));
    ui->scanButton->setStyleSheet(di_.scanEnabled ? kGreen : kOff);
    ui->outputButton->setStyleSheet(di_.outputEnabled ? kGreen : kOff);
    ui->action_Setup_mode->setChecked(di_.setupMode);
    ui->setupButton->setStyleSheet(di_.setupMode ? kYellow : kOff);
    ui->telemetryLabel->setStyleSheet(di_.matrixMonitor ? kYellow : kDisabled);
    ui->insaneLabel->setStyleSheet(di_.controllerInsane ? kRed : kDisabled);
    break;
  default:
    qCritical() << "Unknown device status" << s << "!";
  }
}

void FlightController::lockUI_(bool lock) {
  uiLocked_ = lock;
  ui->macrosButton->setDisabled(lock);
  ui->layoutButton->setDisabled(lock);
  ui->layerModsButton->setDisabled(lock);
  ui->delaysButton->setDisabled(lock);
  ui->hwButton->setDisabled(lock);
  ui->pedalsButton->setEnabled(!lock && di_.config.numPedals);
  ui->thresholdsButton->setEnabled(!lock && di_.config.capabilities.hasThresholds);
  ui->TelemetryButton->setEnabled(!lock && di_.config.capabilities.hasTelemetry);
}

void FlightController::timerEvent(QTimerEvent* timer) {
  if (timer->timerId() == blinkTimerId_) {
    ui->txLabel->setStyleSheet(di_.tx.exchange(false) ? kGreen : kDisabled);
    ui->rxLabel->setStyleSheet(di_.rx.exchange(false) ? kGreen : kDisabled);

    if (di_.latencyMs.size() > 0) {
      ui->latencyLabel->setText(di_.latencyMs);
      di_.latencyMs.clear();
    }
  }
}

// slot wrappers only below, lexicographically sorted --------------------------

void FlightController::resetConnection_() {
  qInfo() << "reconnecting..";
  emit setStatusBit(C2DEVSTATUS_SETUP_MODE, false);
  di_.scheduleDeviceRelease();
}

void FlightController::setMode_(bool isSetup) {
  emit sendCommand(C2CMD_SET_MODE, isSetup ? C2DEVMODE_SETUP : C2DEVMODE_NORMAL);
}

void FlightController::showDelays_() {
  activate_(delays_);
}

void FlightController::showHardware_() {
  activate_(hardware_);
}

void FlightController::showTelemetry_() {
  activate_(telemetry_);
}

void FlightController::showLayers_() {
  activate_(layers_);
}

void FlightController::showLayout_() {
  activate_(layout_);
}

void FlightController::showMacros_() {
  activate_(macros_);
}

void FlightController::showPedals_() {
  activate_(pedals_);
}

void FlightController::showThresholds_() {
  activate_(thresholds_);
}

void FlightController::toggleOutput_() {
  emit flipStatusBit(C2DEVSTATUS_OUTPUT_ENABLED);
}

void FlightController::toggleScan_() {
  emit flipStatusBit(C2DEVSTATUS_SCAN_ENABLED);
}

void FlightController::toggleSetupMode_() {
  emit flipStatusBit(C2DEVSTATUS_SETUP_MODE);
}
