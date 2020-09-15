#pragma once

#include <QFrame>
#include "ui_Hardware.h"

#include "DeviceConfig.h"

class Hardware : public QFrame {
  Q_OBJECT

 public:
  explicit Hardware(DeviceConfig& config);
  ~Hardware() = default;
  void init();

 private:
  void apply_();
  void updateParamVisibility_();

  Ui::Hardware realUi_{};
  Ui::Hardware* ui{&realUi_};

  DeviceConfig& config_;

 private slots:
  void changeMode_(int idx);
};
