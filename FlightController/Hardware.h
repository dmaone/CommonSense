#pragma once

#include <QFrame>
#include "ui_Hardware.h"

#include "DeviceConfig.h"

class Hardware : public QFrame {
  Q_OBJECT

 public:
  explicit Hardware(DeviceConfig& config);
  ~Hardware();
  void init();

 private:
  void _updateParamVisibility();

  Ui::Hardware realUi_{};
  Ui::Hardware* ui{&realUi_};

  DeviceConfig& config_;

 private slots:
  void on_applyButton_clicked();
  void on_modeBox_currentIndexChanged(int idx);
  void on_revertButton_clicked();
};
