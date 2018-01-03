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

#include "../c2/c2_protocol.h"

#include "Delays.h"
#include "DeviceInterface.h"
#include "ExpansionHeader.h"
#include "FirmwareLoader.h"
#include "LayerConditions.h"
#include "LayoutEditor.h"
#include "MatrixMonitor.h"
#include "ThresholdEditor.h"

namespace Ui {
class FlightController;
}

class FlightController : public QMainWindow {
  Q_OBJECT
  Q_ENUMS(StatusPosition)

public:
  explicit FlightController(QWidget *parent = 0);
  ~FlightController();
  void setup(void);
  void show(void);
  LogViewer *getLogViewport(void);
  enum StatusPosition {
    Version,
    Scan,
    Output,
    Setup,
    Monitor,
    Insane,
    StatusPositionMax
  };
  void setOldLogger(QtMessageHandler *logger);
  void logToViewport(const QString &);
  bool eventFilter(QObject *obj __attribute__((unused)), QEvent *event);

signals:
  void sendCommand(c2command cmd, uint8_t msg);

public slots:
  void redButtonToggle(bool);
  void showKeyMonitor(void);
  void statusRequestButtonClick(void);
  void editLayoutClick(void);
  void editMacrosClick(void);
  void editThresholdsClick(void);
  void showLayerConditions(void);
  void mainTabChanged(int);
  void deviceStatusNotification(DeviceInterface::DeviceStatus);

protected:
  void closeEvent(QCloseEvent *);
  virtual void timerEvent(QTimerEvent *);

private:
  Ui::FlightController *ui;
  MatrixMonitor *matrixMonitor;
  LayoutEditor *layoutEditor;
  ThresholdEditor *thresholdEditor;
  LayerConditions *layerConditions;
  Delays *_delays;
  ExpansionHeader *_expHeader;
  FirmwareLoader *loader;
  QtMessageHandler *_oldLogger;
  std::array<QLabel*, StatusPositionMax> _statusDisplay;
  bool _uiLocked = false;
  void lockUI(bool lock);
  void resetTimer(int interval);

private slots:
  void on_action_Setup_mode_triggered(bool bMode);
  void editDelays(void);
  void editExpHeader(void);
};
