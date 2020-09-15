#pragma once

#include <QGridLayout>
#include <QLCDNumber>
#include <QLabel>
#include <QtCore>
#include <stdint.h>
#include "ui_Telemetry.h"

#include "DeviceConfig.h"
#include "DeviceInterface.h"
#include "Events.h"

class Telemetry : public QFrame {
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
  explicit Telemetry(DeviceInterface& di);

  void init();
  void deinit();

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

 protected:
  bool eventFilter(QObject *obj, QEvent *event);
  void closeEvent(QCloseEvent *);

 private:
  Ui::Telemetry realUi_{};
  Ui::Telemetry* ui{&realUi_};

  DisplayMode displayMode_{DisplayNow};
  std::atomic<bool> initialized_{false};
  bool isActive_{false};
  LabelList labels_{};
  std::unique_ptr<QGridLayout> grid_;
  std::vector<Cell> cells_{};
  DeviceInterface& di_;
  uint8_t warmupRows_{ABSOLUTE_MAX_ROWS};

  Cell& getCell_(uint8_t row, uint8_t col);
  void enableReporting_(bool newState);
};
