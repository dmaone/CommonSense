#pragma once

#include <QGridLayout>
#include <QLCDNumber>
#include <QLabel>
#include <QtCore>
#include <stdint.h>
#include "ui_MatrixView.h"

#include "DeviceConfig.h"
#include "DeviceInterface.h"
#include "Events.h"

class MatrixView : public QFrame {
  Q_OBJECT

  struct Cell {
    uint8_t now;
    uint8_t min;
    uint8_t max;
    uint32_t sum;
    uint32_t count;
    QWidget widget{};
    QVBoxLayout subLayout{};
    QLCDNumber readout{};
    QLabel stats{};
  };

  using LabelList = std::vector<std::unique_ptr<QLabel>>;
  using LcdList = std::vector<std::unique_ptr<QLCDNumber>>;

 public:
  explicit MatrixView(DeviceInterface& di);

  void init();
  void disconnect();

  enum DisplayMode { DisplayNow, DisplayMin, DisplayMax, DisplayAvg };
  Q_ENUM(DisplayMode);

 public slots:
  void keypress(DeviceInterface::KeyState state);

 private slots:
  void close_();
  void export_();
  void setDisplayMode_(QString newValue);
  void setThresholds_();
  void resetCells_();
  void run_();

signals:
  void sendCommand(c2command, uint8_t);
  void setStatusBit(deviceStatus, bool);

 protected:
  bool eventFilter(QObject *obj, QEvent *event);
  void closeEvent(QCloseEvent *);

 private:
  Ui::MatrixView realUi_{};
  Ui::MatrixView* ui{&realUi_};

  DisplayMode displayMode{DisplayNow};
  std::atomic<bool> initialized_{false};
  bool isActive_{false};
  LabelList labels_{};
  std::unique_ptr<QGridLayout> grid_;
  std::vector<Cell> cells_{};
  DeviceInterface& di_;
  uint8_t warmupRows_{ABSOLUTE_MAX_ROWS};

  Cell& getCell_(uint8_t row, uint8_t col);
  void enableTelemetry_(bool newState);
};
