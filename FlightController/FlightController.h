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

#include "Delays.h"
#include "DeviceInterface.h"
#include "Hardware.h"
#include "FirmwareLoader.h"
#include "LayerConditions.h"
#include "LayoutEditor.h"
#include "MatrixMonitor.h"
#include "ThresholdEditor.h"
#include "MacroEditor.h"

namespace Ui {
class FlightController;
}

class FlightController : public QMainWindow {
  Q_OBJECT

public:
  explicit FlightController(QWidget *parent = 0);
  ~FlightController();
  void setup();
  void show();
  LogViewer *getLogViewport();
  void setOldLogger(QtMessageHandler *logger);
  void logToViewport(const QString &);

signals:
  void sendCommand(c2command cmd, uint8_t msg);
  void flipStatusBit(deviceStatus bit);
  void setStatusBit(deviceStatus bit, bool newValue);

public slots:
  void showKeyMonitor();
  void editLayoutClick();
  void editMacrosClick();
  void editThresholdsClick();
  void showLayerConditions();
  void deviceStatusNotification(DeviceInterface::DeviceStatus);

protected:
  void closeEvent(QCloseEvent *);

private:
  Ui::FlightController *ui;
  MatrixMonitor *matrixMonitor;
  LayoutEditor *layoutEditor;
  ThresholdEditor *thresholdEditor;
  MacroEditor *macroEditor;
  LayerConditions *layerConditions;
  Delays *_delays;
  Hardware *_hardware;
  FirmwareLoader *loader;
  QtMessageHandler *_oldLogger;
  bool _uiLocked = false;
  int blinkTimerId;

  void lockUI(bool lock);
  void updateStatus();
  void timerEvent(QTimerEvent * timer);
  void blinkLights();

private slots:
  void on_action_Setup_mode_triggered(bool bMode);
  void on_scanButton_clicked();
  void on_outputButton_clicked();
  void on_setupButton_clicked();
  void on_reconnectButton_clicked();
  void editDelays();
  void editHardware();
};
