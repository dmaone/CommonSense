#pragma once

#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include "ui_Thresholds.h"

#include "DeviceConfig.h"
#include "DeviceInterface.h"


class Thresholds : public QFrame {
  Q_OBJECT

  using LabelList = std::vector<std::unique_ptr<QLabel>>;

public:
  explicit Thresholds(DeviceInterface& di);

  void init();

public slots:
  void keypress(DeviceInterface::KeyState state);

protected:
  bool eventFilter(QObject *obj, QEvent *event);

private:
  void apply_();
  void reset_();
  void bumpAll_(int delta);
  QSpinBox& getCell_(uint8_t row, uint8_t col);
  void paintCell_(QSpinBox& cell);

  Ui::Thresholds realUi_{};
  Ui::Thresholds* ui{&realUi_};

  std::unique_ptr<QGridLayout> grid_;
  std::vector<QSpinBox> cells_{};
  LabelList labels_{};
  DeviceInterface& di_;
};
