#include <QComboBox>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QSignalBlocker>
#include <QTextStream>

#include "DeviceInterface.h"
#include "Pedals.h"
#include "ScancodeList.h"
#include "settings.h"

Pedal::Pedal() {
  connect(&edgeDetect, SIGNAL(toggled(bool)), SLOT(updated()));
  connect(&normallyHigh, SIGNAL(toggled(bool)), SLOT(updated()));
  connect(&synced, SIGNAL(toggled(bool)), SLOT(updated()));
}

void Pedal::resetMode(uint8_t newFlags) {
  if (newFlags == EMPTY_FLASH_BYTE) {
    newFlags = 0;
  }
  auto toState = [&newFlags] (auto bit) {
    return newFlags & bit ? Qt::Checked : Qt::Unchecked;
  };
  edgeDetect.setCheckState(toState(PEDAL_EDGE_TRIGGERED));
  normallyHigh.setCheckState(toState(PEDAL_NORMALLY_HIGH));
  synced.setCheckState(toState(PEDAL_LED_SYNCED));
  updateControlsAvailability();
}

void Pedal::updated() {
  updateControlsAvailability();
}

void Pedal::updateControlsAvailability() {
  QSignalBlocker edGuard{edgeDetect};
  QSignalBlocker nhGuard{normallyHigh};
  QSignalBlocker sGuard{synced};
  edgeDetect.setEnabled(true);
  normallyHigh.setEnabled(true);
  synced.setEnabled(true);
  for (auto& c : codes) {
    c->setEnabled(true);
  }

  if (edgeDetect.checkState() == Qt::Checked) {
    normallyHigh.setCheckState(Qt::Unchecked);
    normallyHigh.setEnabled(false);
    normallyHigh.setCheckState(Qt::Unchecked);
    synced.setEnabled(false);
  }
  if (normallyHigh.checkState() == Qt::Checked) {
    edgeDetect.setCheckState(Qt::Unchecked);
    edgeDetect.setEnabled(false);
  }
  if (synced.checkState() == Qt::Checked) {
    edgeDetect.setCheckState(Qt::Unchecked);
    edgeDetect.setEnabled(false);
    auto curValue = codes.front()->currentIndex();
    for (auto& c : codes) {
      c->setCurrentIndex(0);
      c->setEnabled(false);
    }
    codes.front()->setEnabled(true);
    codes.front()->setCurrentIndex(curValue);
  }
}


Pedals::Pedals(DeviceInterface& di) :
    QFrame{nullptr, Qt::Tool}, config_{di.config} {
  ui->setupUi(this);

  connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(apply_()));
  connect(ui->revertButton, SIGNAL(clicked()), this, SLOT(reset_()));
  connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(close()));

  connect(
      &di, SIGNAL(pedal(uint8_t, DeviceInterface::KeyState)),
      this, SLOT(pedal(uint8_t, DeviceInterface::KeyState)));
}

void Pedals::init() {
  if (!config_.numPedals) {
    return;
  }
  LabelList labels;
  std::vector<Pedal> pedals{config_.numPedals};
  grid_ = std::make_unique<QGridLayout>();
  ui->Dashboard->setLayout(grid_.get());
  uint8_t headerCol{1};
  labels.emplace_back(std::make_unique<QLabel>("Edge"));
  grid_->addWidget(labels.back().get(), 0, headerCol++);
  labels.emplace_back(std::make_unique<QLabel>("Inv"));
  grid_->addWidget(labels.back().get(), 0, headerCol++);
  /*
   * TODO: sync LED light to pedal state
  labels.emplace_back(std::make_unique<QLabel>("Sync"));
  grid_->addWidget(labels.back().get(), 0, headerCol++);
  */

  labels.emplace_back(std::make_unique<QLabel>(QString("Base layer")));
  grid_->addWidget(labels.back().get(), 0, headerCol++);

  for (uint8_t l = 1; l < config_.numLayers; ++l) {
    labels.emplace_back(std::make_unique<QLabel>(QString("L%1").arg(l)));
    grid_->addWidget(labels.back().get(), 0, headerCol++);
  }

  ScancodeList scancodes;
  auto makeScancodeBox = [&scancodes] (auto& parent) {
    parent.emplace_back(std::make_unique<QComboBox>());
    auto& box = *parent.back();
    box.setEditable(false);
    box.addItems(*scancodes.list);
    box.setMinimumContentsLength(scancodes.width);
    box.setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    return &box;
  };

  for (uint8_t p = 0; p < config_.numPedals; ++p) {
    labels.emplace_back(std::make_unique<QLabel>(QString("%1").arg(p + 1)));
    grid_->addWidget(labels.back().get(), p + 1, 0, 1, 1, Qt::AlignRight);
    uint8_t curCol{1};
    auto& obj = pedals[p];
    grid_->addWidget(&obj.edgeDetect, p + 1, curCol++);
    grid_->addWidget(&obj.normallyHigh, p + 1, curCol++);
    /* TODO: sync LED light to pedal state
    grid_->addWidget(&obj.synced, p + 1, curCol++);
    */
    for (uint8_t l = 0; l < config_.numLayers; ++l) {
      grid_->addWidget(makeScancodeBox(obj.codes), p + 1, curCol++);
    }
  }

  labels_.swap(labels);
  pedals_.swap(pedals);
  reset_();
  adjustSize();
}


void Pedals::apply_() {
  for (uint8_t p = 0; p < config_.numPedals; ++p) {
    auto& obj = pedals_[p];
    auto syncMode = obj.synced.isChecked();
    config_.pedalFlags[p] = (syncMode ? PEDAL_LED_SYNCED : 0)
        + (obj.edgeDetect.isChecked() ? PEDAL_EDGE_TRIGGERED : 0)
        + (obj.normallyHigh.isChecked() ? PEDAL_NORMALLY_HIGH : 0);

    auto baseCode = obj.codes.front()->currentIndex();
    for (uint8_t l = 0; l < config_.numLayers; ++l) {
      config_.pedals[p][l] = syncMode ? baseCode : obj.codes[l]->currentIndex();
    }
  }
}

void Pedals::reset_() {
  for (uint8_t p = 0; p < config_.numPedals; ++p) {
    auto& obj = pedals_.at(p);
    obj.resetMode(config_.pedalFlags[p]);
    for (uint8_t l = 0; l < config_.numLayers; ++l) {
      obj.codes[l]->setCurrentIndex(config_.pedals[p][l]);
    }
  }
}

void Pedals::pedal(uint8_t pedalIndex, DeviceInterface::KeyState state) {
  if (pedals_.empty()) {
    return; // never happens, but..
  }
  auto& cell = pedals_.at(pedalIndex).codes.front();
  if (state != DeviceInterface::KeyPressed) {
    cell->setStyleSheet("");
    return;
  }
  cell->setStyleSheet("color: black; background-color: #ffff33;");
  cell->setFocus();
}
