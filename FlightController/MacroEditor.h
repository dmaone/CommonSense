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
  void show();
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
  int contextMenuRow;

private slots:
  void on_closeButton_clicked();
  void on_resetButton_clicked();
  void on_revertButton_clicked();
  void on_addButton_clicked();
  void on_deleteButton_clicked();
  void on_macroListCombo_currentIndexChanged(int);
  void addStepButtonClicked();
  void cmdIndexChanged(int);
  void userChanged();
  void showContextMenu(QPoint);
  void contextMenuInsertTriggered();
  void contextMenuDeleteTriggered();
};
