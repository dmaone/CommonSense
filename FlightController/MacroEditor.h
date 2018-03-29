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
  QByteArray encodeSteps(int row);
  void populateSteps(QByteArray& bytes);
  void addStep(int row);
  Ui::MacroEditor *ui;
  DeviceConfig *deviceConfig;
  int currentMacro{0};
  bool changed{false};
  bool ignoreOnce{false};

private slots:
  void on_closeButton_clicked();
  void on_applyButton_clicked();
  void on_revertButton_clicked();
  void on_addButton_clicked();
  void on_deleteButton_clicked();
  void on_macroListCombo_currentIndexChanged(int);
  void addStepButtonClicked();
  void cmdIndexChanged(int idx);
  void userChanged();
};
