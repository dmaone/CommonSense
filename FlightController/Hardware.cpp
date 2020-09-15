#include "Hardware.h"

namespace {
constexpr size_t kMaxDebouncingTicks{64};
}

Hardware::Hardware(DeviceConfig& config) :
    QFrame{nullptr, Qt::Tool}, config_{config} {
  ui->setupUi(this);
  for (const auto& it: config_.getExpModeNames()) {
    ui->modeBox->addItem(it.data());
  }
}

Hardware::~Hardware() { delete ui; }

void Hardware::init() {
  auto config = config_.getHardwareConfig();
  switch (config.adcBits) {
    case 8:
      ui->adcBits->setCurrentIndex(0);
      break;
    case 10:
      ui->adcBits->setCurrentIndex(1);
      break;
    case 12:
      ui->adcBits->setCurrentIndex(2);
      break;
    default:
      qInfo() << "Unknown ADC resolution: " << config.adcBits << "!!!";
  }

  ui->chargeDelay->setValue(config.chargeDelay);
  ui->dischargeDelay->setValue(config.dischargeDelay);
  ui->debouncingTicks->setValue(config.debouncingTicks);
  ui->debouncingTicks->setRange(1, kMaxDebouncingTicks);

  ui->adcBits->setEnabled(config_.capabilities.hasMatrixView);
  ui->chargeDelay->setEnabled(config_.capabilities.hasChargeSequencer);
  ui->dischargeDelay->setEnabled(config_.capabilities.hasChargeSequencer);

  ui->modeBox->setCurrentIndex(config.expHdrMode);
  ui->Param1->setValue(config.expHdrParam1);
  ui->Param2->setValue(config.expHdrParam2);
}

void Hardware::_updateParamVisibility() {
  // Nothing - both modes blink something on keypress now.
}

void Hardware::on_modeBox_currentIndexChanged(int idx) {
  Q_UNUSED(idx);
  _updateParamVisibility();
}

void Hardware::on_applyButton_clicked() {
  std::vector<uint8_t> adcBitMap {8, 10, 12};
  HardwareConfig config;
  config.adcBits = adcBitMap[ui->adcBits->currentIndex()];
  config.chargeDelay = ui->chargeDelay->value();
  config.dischargeDelay = ui->dischargeDelay->value();
  config.debouncingTicks = ui->debouncingTicks->value();
  config.expHdrMode = ui->modeBox->currentIndex();
  config.expHdrParam1 = ui->Param1->value();
  config.expHdrParam2 = ui->Param2->value();
  config_.setHardwareConfig(config);
}

void Hardware::on_revertButton_clicked() {
  init();
}
