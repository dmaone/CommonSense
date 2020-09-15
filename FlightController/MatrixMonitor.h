#pragma once

#include <QGridLayout>
#include <QLCDNumber>
#include <QLabel>
#include <QtCore>
#include <stdint.h>
#include "ui_MatrixMonitor.h"

#include "DeviceConfig.h"
#include "DeviceInterface.h"
#include "Events.h"

typedef struct {
  uint8_t now;
  uint8_t min;
  uint8_t max;
  uint32_t sum;
  uint32_t count;
} MonitoredCell;

class MatrixMonitor : public QFrame {
  Q_OBJECT

public:
  explicit MatrixMonitor(DeviceInterface& di);

  void show();
  enum DisplayMode { DisplayNow, DisplayMin, DisplayMax, DisplayAvg };
  Q_ENUM(DisplayMode);

public slots:
  void keypress(DeviceInterface::KeyState state);

signals:
  void sendCommand(c2command, uint8_t);
  void setStatusBit(deviceStatus, bool);

protected:
  bool eventFilter(QObject *obj, QEvent *event);
  void closeEvent(QCloseEvent *);

private:
  Ui::MatrixMonitor realUi_{};
  Ui::MatrixMonitor* ui{&realUi_};

  uint8_t debug;
  DisplayMode displayMode{DisplayNow};
  QGridLayout grid_{};
  QLCDNumber *display[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
  MonitoredCell cells[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
  QLabel *statsDisplay[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
  DeviceInterface& di_;
  uint8_t warmupRows_{ABSOLUTE_MAX_ROWS};

  void initDisplay();
  void updateDisplaySize(uint8_t, uint8_t);
  void enableTelemetry(uint8_t);
  void _resetCells();
  void _updateStatCell(uint8_t row, uint8_t col, uint8_t level);
  void _updateStatCellDisplay(uint8_t row, uint8_t col);

private slots:
  void on_runButton_clicked();
  void on_closeButton_clicked();
  void on_setThresholdsButton_clicked();
  void on_modeBox_currentTextChanged(QString newValue);
  void on_resetButton_clicked();
  void on_exportButton_clicked();
};
