#include "Hardware.h"
#include "ui_Hardware.h"

Hardware::Hardware(DeviceConfig *config, QWidget *parent)
    : QFrame(parent), ui(new Ui::Hardware), _config(config) {
  ui->setupUi(this);
  for (uint8_t i = 0; i < (sizeof expModeNames / sizeof expModeNames[0]); i++) {
    ui->modeBox->addItem(expModeNames[i]);
  }
}

Hardware::~Hardware() { delete ui; }

void Hardware::init() {
  auto config = _config->getHardwareConfig();
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
  ui->modeBox->setCurrentIndex(config.expHdrMode);
  ui->Param1->setValue(config.expHdrParam1);
  ui->Param2->setValue(config.expHdrParam2);
}

void Hardware::_updateParamVisibility() {
  if (ui->modeBox->currentText().contains("Solenoid")) {
    ui->Param1Label->setText("Drive time, ms");
    ui->Param1Label->setEnabled(true);
    ui->Param2Label->setText("Cooldown time, ms");
    ui->Param2Label->setEnabled(true);
    ui->Param1->setEnabled(true);
    ui->Param2->setEnabled(true);
  } else {
    ui->Param1Label->setText("Unused");
    ui->Param1Label->setEnabled(false);
    ui->Param2Label->setText("Unused");
    ui->Param2Label->setEnabled(false);
    ui->Param1->setEnabled(false);
    ui->Param2->setEnabled(false);
  }
}

void Hardware::on_modeBox_currentIndexChanged(int idx) {
  Q_UNUSED(idx);
  _updateParamVisibility();
}

void Hardware::on_applyButton_clicked(void) {
  std::vector<uint8_t> adcBitMap {8, 10, 12};
  HardwareConfig config;
  config.adcBits = adcBitMap[ui->adcBits->currentIndex()];
  config.chargeDelay = ui->chargeDelay->value();
  config.dischargeDelay = ui->dischargeDelay->value();
  config.debouncingTicks = ui->debouncingTicks->value();
  config.expHdrMode = ui->modeBox->currentIndex();
  config.expHdrParam1 = ui->Param1->value();
  config.expHdrParam2 = ui->Param2->value();
  _config->setHardwareConfig(config);
}

void Hardware::on_revertButton_clicked(void) {
  init();
}
