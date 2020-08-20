#pragma once

#include "DeviceConfig.h"
#include "DeviceInterface.h"
#include <QCheckBox>
#include <QFrame>
#include <QGridLayout>
#include <QSpinBox>

namespace Ui {
class ThresholdEditor;
}

class ThresholdEditor : public QFrame {
  Q_OBJECT

public:
  explicit ThresholdEditor(DeviceConfig *config, QWidget *parent = 0);
  ~ThresholdEditor();
  void show();

public slots:
  void applyThresholds();
  void resetThresholds();
  void increaseThresholds();
  void decreaseThresholds();
  void receiveScancode(uint8_t row, uint8_t col,
                       DeviceInterface::KeyStatus status);

protected:
  bool eventFilter(QObject *obj, QEvent *event);

private:
  Ui::ThresholdEditor *ui;
  QGridLayout *grid;
  QSpinBox *display[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
  DeviceConfig *deviceConfig;
  void initDisplay();
  void updateDisplaySize(uint8_t, uint8_t);
  void adjustThresholds(size_t delta);
  void paintCell(QSpinBox *cell);

private slots:
  void on_closeButton_clicked();
};
