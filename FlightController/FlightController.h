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
#include "ui_FlightController.h"

#include "Delays.h"
#include "DeviceInterface.h"
#include "FirmwareLoader.h"
#include "Hardware.h"
#include "LayerConditions.h"
#include "Layout.h"
#include "Macros.h"
#include "MatrixView.h"
#include "Thresholds.h"

class FlightController : public QMainWindow {
  Q_OBJECT

 public:
  explicit FlightController();
  ~FlightController();

  void setup();

 signals:
  void sendCommand(c2command cmd, uint8_t msg);
  void flipStatusBit(deviceStatus bit);
  void setStatusBit(deviceStatus bit, bool newValue);

 public slots:
  void deviceStatusNotification(DeviceInterface::DeviceStatus);

 private slots:
  void setMode_(bool isSetup);
  void showDelays_();
  void showHardware_();
  void showMatrixView_();
  void showLayers_();
  void showLayout_();
  void showMacros_();
  void showThresholds_();
  void resetConnection_();
  void toggleOutput_();
  void toggleScan_();
  void toggleSetupMode_();

 private:
  template <class T>
  void activate_(T& form) {
    form.show();
    form.raise();
  }

  void connectBackendSlots_();
  void connectUiSlots_();
  void lockUI_(bool lock);
  void timerEvent(QTimerEvent* timer);

  Ui::FlightController realUi_{};
  Ui::FlightController* ui{&realUi_};
  DeviceInterface di_{};
  Delays delays_;
  Hardware hardware_;
  LayerConditions layers_;
  Layout layout_;
  FirmwareLoader loader_;
  Macros macros_;
  MatrixView matrixView_;
  Thresholds thresholds_;
  bool uiLocked_{false};
  int blinkTimerId_{0};


};
