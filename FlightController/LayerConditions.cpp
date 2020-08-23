
#include "LayerConditions.h"

LayerConditionWatcher::LayerConditionWatcher(
    DeviceConfig& config, int conditionIndex, QCheckBox *fn1Check,
    QCheckBox *fn2Check, QCheckBox *fn3Check, QCheckBox *fn4Check,
    QComboBox *layerCombo)
    : config_{config}, conditionIndex(conditionIndex),
      fn1Check(fn1Check), fn2Check(fn2Check), fn3Check(fn3Check),
      fn4Check(fn4Check), layerCombo(layerCombo) {
  connect(fn1Check, SIGNAL(toggled(bool)), SLOT(changed()));
  connect(fn2Check, SIGNAL(toggled(bool)), SLOT(changed()));
  connect(fn3Check, SIGNAL(toggled(bool)), SLOT(changed()));
  connect(fn4Check, SIGNAL(toggled(bool)), SLOT(changed()));
  connect(layerCombo, SIGNAL(currentIndexChanged(int)), SLOT(changed()));
}

LayerCondition LayerConditionWatcher::toLayerCondition() {
  return LayerCondition(fn1Check->isChecked(), fn2Check->isChecked(),
                        fn3Check->isChecked(), fn4Check->isChecked(),
                        layerCombo->currentIndex());
}

void LayerConditionWatcher::changed() {
  config_.setLayerCondition(conditionIndex, toLayerCondition());
}

LayerConditions::LayerConditions(DeviceConfig& config)
    : QWidget(nullptr, Qt::Tool), config_{config} {
  setLayout(&grid_);
  init();
}

void LayerConditions::init() {
  auto cnds = config_.loadLayers();
  watchers_.clear();
  boxes_.clear();
  combos_.clear();
  labels_.clear();

  int count = cnds.size();
  for (int i = 0; i < NUM_FNS; i++) {
    auto label = std::make_unique<QLabel>(QString("<b>Fn%1</b>").arg(i + 1));
    grid_.addWidget(label.get(), 0, i + 2);
    labels_.emplace_back(std::move(label));
  }
  for (int i = 0; i < count; i++) {
    auto label = std::make_unique<QLabel>(QString("<b>%1</b>").arg(i + 1));
    grid_.addWidget(label.get(), i + 1, 1);
    labels_.emplace_back(std::move(label));

    auto fn1Check = std::make_unique<QCheckBox>();
    fn1Check->setChecked(cnds[i].fn1Set());

    auto fn2Check = std::make_unique<QCheckBox>();
    fn2Check->setChecked(cnds[i].fn2Set());

    auto fn3Check = std::make_unique<QCheckBox>();
    fn3Check->setChecked(cnds[i].fn3Set());

    auto fn4Check = std::make_unique<QCheckBox>();
    fn3Check->setChecked(cnds[i].fn4Set());

    auto arrowLabel = std::make_unique<QLabel>(QChar(0x2192));

    auto combo = std::make_unique<QComboBox>();
    for (int j = 0; j < config_.numLayers; j++)
      combo->addItem(j == 0 ? "Base Layer" : QString("Layer %1").arg(j), j);
    combo->setCurrentIndex(cnds[i].layer());

    grid_.addWidget(fn1Check.get(), i + 1, 2);
    grid_.addWidget(fn2Check.get(), i + 1, 3);
    grid_.addWidget(fn3Check.get(), i + 1, 4);
    grid_.addWidget(fn4Check.get(), i + 1, 5);
    grid_.addWidget(arrowLabel.get(), i + 1, 6);
    grid_.addWidget(combo.get(), i + 1, 7);

    watchers_.emplace_back(std::make_unique<LayerConditionWatcher>(
        config_, i,
        fn1Check.get(), fn2Check.get(), fn3Check.get(), fn4Check.get(),
        combo.get()));
    boxes_.emplace_back(std::move(fn1Check));
    boxes_.emplace_back(std::move(fn2Check));
    boxes_.emplace_back(std::move(fn3Check));
    boxes_.emplace_back(std::move(fn4Check));
    combos_.emplace_back(std::move(combo));
    labels_.emplace_back(std::move(arrowLabel));
    labels_.emplace_back(std::move(label));
  }

  grid_.setRowStretch(grid_.rowCount(), 1);
  grid_.setColumnStretch(0, 4);
  for (int i = 1; i < grid_.columnCount(); i++)
    grid_.setColumnStretch(i, 1);
  grid_.setColumnStretch(grid_.columnCount(), 4);
  setWindowTitle("Layer Mods");
}
