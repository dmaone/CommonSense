#pragma once

#include <QComboBox>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <stdint.h>

#include "ui_Layout.h"

#include "DeviceConfig.h"
#include "DeviceInterface.h"

class Layout : public QFrame {
  Q_OBJECT

  using LabelList = std::vector<std::unique_ptr<QLabel>>;
  using ComboList = std::vector<std::unique_ptr<QComboBox>>;

 public:
  explicit Layout(DeviceInterface& di);

  void init();

 public slots:
  void receiveScancode(
      uint8_t row, uint8_t col, DeviceInterface::KeyStatus status);

 private slots:
  void import_();
  void export_();
  void apply_();
  void reset_();
  void switchLayer_();

 private:
  void syncUiMatrix_();

  QComboBox& getUiCell_(uint8_t row, uint8_t col);

  Ui::Layout realUi_{};
  Ui::Layout* ui{&realUi_};
  LabelList labels_{};
  ComboList display_{};
  std::unique_ptr<QGridLayout> grid_;
  DeviceConfig& config_;
  uint8_t currentLayer_{0};
  QString defaultCss_{};

};
