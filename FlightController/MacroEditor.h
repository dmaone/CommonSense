#pragma once

#include "DeviceConfig.h"
#include "DeviceInterface.h"
#include <QCheckBox>
#include <QFrame>
#include <QGridLayout>
#include <QSpinBox>

namespace Ui {
class MacroEditor;
}

class MacroEditor : public QFrame {
  Q_OBJECT

public:
  explicit MacroEditor(DeviceConfig *config, QWidget *parent = 0);
  ~MacroEditor();
  void show(void);
  int findWidgetRow(QWidget *w);
  void fillCommandParameters(int row, int command);

private:
  Ui::MacroEditor *ui;
  DeviceConfig *deviceConfig;

private slots:
  void on_closeButton_clicked();
  void on_applyButton_clicked();
  void on_revertButton_clicked();
  void on_addButton_clicked();
  void on_deleteButton_clicked();
  void on_macroListCombo_currentIndexChanged(int);
  void on_addStepButton_clicked();
  void cmdIndexChanged(int idx);
};
