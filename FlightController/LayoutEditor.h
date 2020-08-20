#pragma once

#include <QComboBox>
#include <QFrame>
#include <QGridLayout>
#include <stdint.h>

#include "DeviceConfig.h"
#include "DeviceInterface.h"

namespace Ui {
class LayoutEditor;
}

class LayoutEditor : public QFrame {
  Q_OBJECT

public:
  explicit LayoutEditor(DeviceConfig *config, QWidget *parent = 0);
  ~LayoutEditor();
  void show();

public slots:
  void importLayout();
  void exportLayout();
  void applyLayout();
  void resetLayout();
  void switchLayer();
  void receiveScancode(uint8_t row, uint8_t col,
                       DeviceInterface::KeyStatus status);

private:
  Ui::LayoutEditor *ui;
  QGridLayout *grid;
  QComboBox *display[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
  DeviceConfig *deviceConfig;
  uint8_t currentLayer{0};
  QString defaultCss_{};
  void initDisplay();
  void sizeDisplay(uint8_t, uint8_t);
  void setDisplay();

private slots:
  void on_closeButton_clicked();
};
