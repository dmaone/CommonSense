#pragma once

#include <QCheckBox>
#include <QFrame>
#include <QGridLayout>
#include <QSpinBox>
#include "ui_Macros.h"

#include "DeviceConfig.h"
#include "DeviceInterface.h"

class Macros : public QFrame {
  Q_OBJECT

 public:
  explicit Macros(DeviceConfig& config);

  void show();
  int findWidgetRow(QWidget *w);
  void fillCommandParameters(int row, int command);

 private:
  QByteArray encodeSteps(int row);
  void populateSteps(QByteArray& bytes);
  void addStep(int row);

  Ui::Macros realUi_{};
  Ui::Macros* ui{&realUi_};

  DeviceConfig& config_;
  int currentMacro{0};
  bool changed{false};
  bool ignoreOnce{false};
  int contextMenuRow;

 private slots:
  void addStepButtonClicked();
  void cmdIndexChanged(int);
  void contextMenuInsertTriggered();
  void contextMenuDeleteTriggered();
  void on_addButton_clicked();
  void on_closeButton_clicked();
  void on_deleteButton_clicked();
  void on_macroListCombo_currentIndexChanged(int);
  void on_resetButton_clicked();
  void on_revertButton_clicked();
  void showContextMenu(QPoint);
  void userChanged();
};
