#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QWidget>

#include "DeviceConfig.h"
#include "LayerCondition.h"

#define NUM_FNS 4

class LayerConditionWatcher : public QObject {
  Q_OBJECT

 public:
  LayerConditionWatcher(DeviceConfig& config, int conditionIndex,
                        QCheckBox *fn1Check, QCheckBox *fn2Check,
                        QCheckBox *fn3Check, QCheckBox *fn4Check,
                        QComboBox *layerCombo);
  LayerCondition toLayerCondition();

 private:
  DeviceConfig& config_;
  int conditionIndex;
  QCheckBox *fn1Check;
  QCheckBox *fn2Check;
  QCheckBox *fn3Check;
  QCheckBox *fn4Check;
  QComboBox *layerCombo;

 private slots:
  void changed();
};

class LayerConditions : public QWidget {
  Q_OBJECT
 public:
  LayerConditions(DeviceConfig& config);
  void init();

  std::vector<std::unique_ptr<LayerConditionWatcher>> watchers_;

 private:
  DeviceConfig& config_;
  QGridLayout grid_;
  std::vector<std::unique_ptr<QCheckBox>> boxes_{};
  std::vector<std::unique_ptr<QComboBox>> combos_{};
  std::vector<std::unique_ptr<QLabel>> labels_{};
};
