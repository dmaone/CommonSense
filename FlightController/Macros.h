#pragma once

#include <QFrame>
#include "ui_Macros.h"

#include "DeviceConfig.h"

class Macros : public QFrame {
  Q_OBJECT

 public:
  explicit Macros(DeviceConfig& config);

  void show();
  void fillCommandParameters(int row, int command);

 private:
  QByteArray encodeSteps_(int row);
  void populateSteps_(QByteArray& bytes);
  void addStep_(int row);
  void appendMacro_();

  Ui::Macros realUi_{};
  Ui::Macros* ui{&realUi_};

  DeviceConfig& config_;
  int currentMacro_{0};
  bool dirty_{false};
  int contextMenuRow_;
  const QString kNew{"-new-"};


 private slots:
  void appendStep_();
  void setTriggerMode_(int);
  void insertStep_();
  void deleteStep_();
  void deleteCurrentMacro_();
  void selectMacro_(int);
  void reset_();
  void contextMenu_(QPoint);
  void setDirty_();
};
