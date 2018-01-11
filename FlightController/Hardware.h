#pragma once

#include <QFrame>

#include "DeviceConfig.h"

namespace Ui {
class Hardware;
}

class Hardware : public QFrame {
  Q_OBJECT

public:
  explicit Hardware(DeviceConfig *config, QWidget *parent = 0);
  ~Hardware();
  void init(void);

private:
  Ui::Hardware *ui;
  DeviceConfig *_config;
  void _updateParamVisibility(void);

private slots:
  void on_modeBox_currentIndexChanged(int idx);
  void on_applyButton_clicked(void);
  void on_revertButton_clicked(void);
};
