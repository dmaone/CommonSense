#include "DeviceSelector.h"

#include <QDebug>

DeviceSelector::DeviceSelector(DeviceList& list, QWidget *parent) :
  QDialog(parent), layout_(new QVBoxLayout), label_(new QLabel) {
  label_->setText("Select a device:");
  layout_->addWidget(label_.get());
  size_t i{0};
  for (const auto& sn : list) {
    buttons_.emplace_back(std::make_unique<QPushButton>(sn.first));
    connect(
      buttons_.back().get(),
      &QPushButton::clicked,
      this,
      [this, i](){ onClick(i); });
    layout_->addWidget(buttons_.back().get());
    ++i;
  }
  setLayout(layout_.get());
  setWindowTitle("Select a device");
}

void DeviceSelector::onClick(size_t idx) {
  selectedIndex = idx;
  accept();
}
