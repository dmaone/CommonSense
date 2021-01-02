#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <stdint.h>
#include <atomic>

#include "ui_Pedals.h"

#include "DeviceConfig.h"
#include "DeviceInterface.h"

class Pedal : public QObject {
  Q_OBJECT

 public:
  explicit Pedal();
  void resetMode(uint8_t newFlags);

 public slots:
  void updated();

 public:
  QCheckBox edgeDetect{};
  QCheckBox normallyHigh{};
  QCheckBox synced{};
  std::vector<std::unique_ptr<QComboBox>> codes{};

 private:
  std::atomic<bool> initializing_{false};
  void updateControlsAvailability();
};

class Pedals : public QFrame {
  Q_OBJECT

  using LabelList = std::vector<std::unique_ptr<QLabel>>;

 public:
  explicit Pedals(DeviceInterface& di);

  void init();

 public slots:
  void pedal(uint8_t keyIndex, DeviceInterface::KeyState state);

 private slots:
  void apply_();
  void reset_();

 private:
  Ui::Pedals realUi_{};
  Ui::Pedals* ui{&realUi_};
  LabelList labels_;
  std::vector<Pedal> pedals_;
  std::unique_ptr<QGridLayout> grid_;
  DeviceConfig& config_;
};
