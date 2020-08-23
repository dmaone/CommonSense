#pragma once

#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QWidget>

#include "DeviceConfig.h"

class DelayWatcher : public QObject {
  Q_OBJECT

 public:
  DelayWatcher(DeviceConfig& config, int pos, QSpinBox& box);
  uint16_t getValue();

 private:
  DeviceConfig& config_;
  int pos_;

 private slots:
  void changed(int delay_ms);
};

class Delays : public QWidget {
  Q_OBJECT
 public:
  explicit Delays(DeviceConfig& config);
  ~Delays() = default;
  void init();

  using WatcherList = std::vector<std::unique_ptr<DelayWatcher>>;
  WatcherList watchers;

 private:
  using LabelList = std::vector<std::unique_ptr<QLabel>>;
  using SpinBoxList = std::vector<std::unique_ptr<QSpinBox>>;

  DeviceConfig& config_;
  LabelList rowLabels_{};
  SpinBoxList delays_{};
  std::unique_ptr<QGridLayout> grid_{nullptr};
};
