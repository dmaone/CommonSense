#pragma once

#include <QComboBox>
#include <QFrame>
#include <QGridLayout>
#include <stdint.h>
#include "ui_Pedals.h"

#include "DeviceConfig.h"
#include "DeviceInterface.h"

class Pedals : public QFrame {
  Q_OBJECT

public:
  explicit Pedals(DeviceConfig& config);
  void show();

public slots:
  void apply();
  void revert();
  void receiveScancode(uint8_t row, uint8_t col,
                       DeviceInterface::KeyStatus status);

private:
  Ui::Pedals realUi_{};
  Ui::Pedals* ui{&realUi_};
  QGridLayout grid_{};
  QComboBox *display[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
  DeviceConfig& config_;
  void initDisplay();
  void sizeDisplay(uint8_t, uint8_t);
  void setDisplay();

private slots:
  void on_closeButton_clicked();
};
