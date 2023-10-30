#include "Delays.h"

DelayWatcher::DelayWatcher(
    DeviceConfig& config, int pos, QSpinBox& box):
        config_{config}, pos_{pos} {
  connect(&box, SIGNAL(valueChanged(int)), SLOT(changed(int)));
}

void DelayWatcher::changed(int delay_ms) {
  config_.setDelay(pos_, delay_ms);
}

Delays::Delays(DeviceConfig& config)
    : QWidget{nullptr, Qt::Tool}, config_{config} {
  setWindowTitle("Delays");
  setMinimumWidth(150);
}

void Delays::init() {
  watchers.clear();
  LabelList labels;
  SpinBoxList delays;

  grid_ = std::make_unique<QGridLayout>();
  for (size_t i = 0; i < config_.numDelays; ++i) {
    labels.emplace_back(std::make_unique<QLabel>());
    auto& label = *labels.back();
    switch (i) {
     case 0:
      label.setText(QString("Event delay"));
      label.setToolTip(QString("Keyboard will type no faster than 1 key per period"));
      break;
     case 1:
      label.setText(QString("Tap delay"));
      break;
     default:
      label.setText(QString("%1").arg(i));
      break;
    }
    label.setAlignment(Qt::AlignRight | Qt::AlignCenter);
    grid_->addWidget(&label, i, 0);

    delays.emplace_back(std::make_unique<QSpinBox>());
    auto& box = *delays.back();
    box.setMaximum(UINT16_MAX);
    box.setValue(config_.getDelay(i));
    grid_->addWidget(&box, i, 1);
    watchers.emplace_back(std::make_unique<DelayWatcher>(config_, i, box));
  }
  setLayout(grid_.get());
  adjustSize();

  rowLabels_.swap(labels);
  delays_.swap(delays);

}
