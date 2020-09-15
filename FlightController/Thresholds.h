#pragma once

#include <QCheckBox>
#include <QFrame>
#include <QGridLayout>
#include <QSpinBox>
#include "ui_Thresholds.h"

#include "DeviceConfig.h"
#include "DeviceInterface.h"


class Thresholds : public QFrame {
  Q_OBJECT

public:
  explicit Thresholds(DeviceInterface& di);

  void show();

public slots:
  void applyThresholds();
  void resetThresholds();
  void increaseThresholds();
  void decreaseThresholds();
  void keypress(DeviceInterface::KeyState state);

protected:
  bool eventFilter(QObject *obj, QEvent *event);

private:
  Ui::Thresholds realUi_{};
  Ui::Thresholds* ui{&realUi_};

  QGridLayout _grid{};
  QSpinBox *display[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
  DeviceInterface& di_;
  void initDisplay();
  void updateDisplaySize(uint8_t, uint8_t);
  void adjustThresholds(size_t delta);
  void paintCell(QSpinBox *cell);

private slots:
  void on_closeButton_clicked();
};
