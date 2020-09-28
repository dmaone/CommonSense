#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

#include "DeviceConfig.h"
#include "DeviceInterface.h"
#include "LayerCondition.h"
#include "settings.h"

namespace {

static const std::vector<std::string> expModeNames_{
  "Disabled",

  "Solenoid+Num+Caps", "Lock LEDs",
};

static const std::vector<std::string> switchTypeNames_ {
  "CapInverted", "Capacitive", "ADB", "Sun", "Inductive", "Microswitch", "UNKNOWN"
};

} //namespace

DeviceConfig::DeviceConfig(DeviceInterface* di) : interface_{di} {
  reset();
}

void DeviceConfig::reset() {
  memset(this->eeprom_.raw, 0x00, sizeof(this->eeprom_));
  bValid = false;
  transferDirection_ = TransferIdle;
  currentBlock_ = 0;
}

bool DeviceConfig::eventFilter(QObject* /* obj */, QEvent *event) {
  if (event->type() != DeviceMessage::ET) {
    return false;
  }
  QByteArray *payload = static_cast<DeviceMessage *>(event)->getPayload();
  if (payload->at(0) != C2RESPONSE_CONFIG)
    return false;

  currentBlock_++;
  switch (transferDirection_) {
  case TransferDownload:
    receiveConfigBlock_(payload);
    break;
  case TransferUpload:
    sendConfigBlock_();
    break;
  default:
    qInfo() << "Received config block" << ((uint8_t)payload->at(1))
            << "while supposed to be idle!";
  }
  return true;
}

/**
 * @brief DeviceInterface::uploadConfig
 * Fire up the uploader.
 */
void DeviceConfig::toDevice() {
  if (interface_->getStatusBit(C2DEVSTATUS_TELEMETRY_MODE)) {
    QMessageBox::critical(
        nullptr, "Telemetry active", "Turn off telemetry first!");
    return;
  }
  if (transferDirection_ != TransferIdle) {
    qInfo() << "Not a good day to upload config!";
    QMessageBox::critical(nullptr, "Not a good day to upload config",
                          "Error! Try pressing 'Reconnect' button!");
    return;
  }
  this->assemble_();
  emit sendCommand(C2CMD_EWO, (1 << C2DEVSTATUS_SETUP_MODE));
  this->transferDirection_ = TransferUpload;
  this->currentBlock_ = 0;
  qInfo() << "Uploading config";
  this->sendConfigBlock_();
}

/**
 * @brief DeviceInterface::sendConfigBlock
 * Uploads block pointed to by currentBlock_ to device.
 */
void DeviceConfig::sendConfigBlock_() {
  switch (transferDirection_) {
  case TransferUpload:
    if (currentBlock_ > (EEPROM_BYTESIZE / CONFIG_TRANSFER_BLOCK_SIZE)) {
      qInfo() << "done!";
      transferDirection_ = TransferIdle;
      emit sendCommand(C2CMD_APPLY_CONFIG, 1);
      return;
    }
    qInfo(".");
    OUT_c2packet_t msg;
    msg.command = C2CMD_UPLOAD_CONFIG;
    msg.payload[0] = currentBlock_;
    memcpy(msg.payload + CONFIG_BLOCK_DATA_OFFSET,
           this->eeprom_.raw + (CONFIG_TRANSFER_BLOCK_SIZE * currentBlock_),
           CONFIG_TRANSFER_BLOCK_SIZE);
    emit uploadBlock(msg);
    break;
  default:
    qInfo() << "Not a good day to upload config block!";
    QMessageBox::critical(nullptr, "Not a good day to upload config block",
                          "Error! Try pressing 'Reconnect' button!");
  }
}

void DeviceConfig::fromDevice() {
  if (interface_->getStatusBit(C2DEVSTATUS_TELEMETRY_MODE)) {
    QMessageBox::critical(
      nullptr, "Telemetry active", "Turn off telemetry first!");
    return;
  }
  switch (transferDirection_) {
  case TransferIdle:
    transferDirection_ = TransferDownload;
    currentBlock_ = 0;
    qInfo() << "Downloading config";
    qInfo() << ".";
    break;
  case TransferDownload:
    qInfo() << "Already downloading! Re-requesting current block just in case";
    break;
  default:
    qInfo() << "Not a good day to download config!";
    QMessageBox::critical(nullptr, "Not a good day to download config",
                          "Error! Try pressing 'Reconnect' button!");
    return;
  }
  emit downloadBlock(C2CMD_DOWNLOAD_CONFIG, currentBlock_);
}

/**
 * @brief DeviceInterface::_downloadConfigBlock
 * Receives one block from device, writes it to local config.F
 * @param payload - packet payload
 */
void DeviceConfig::receiveConfigBlock_(QByteArray *payload) {
  if (transferDirection_ != TransferDownload) {
    qInfo() << "Not a good day to download config block!";
    QMessageBox::critical(nullptr, "Not a good day to download config blockb",
                          "Error! Try pressing 'Reconnect' button!");
    return;
  }
  qInfo(".");
  if (currentBlock_ >= (EEPROM_BYTESIZE / CONFIG_TRANSFER_BLOCK_SIZE)) {
    transferDirection_ = TransferIdle;
    qInfo() << "done, unpacking...";
    unpack_();
    return;
  }
  memcpy(this->eeprom_.raw +
             (CONFIG_TRANSFER_BLOCK_SIZE * (uint8_t)payload->at(1)),
         payload->data() + 1 + CONFIG_BLOCK_DATA_OFFSET,
         CONFIG_TRANSFER_BLOCK_SIZE);
  emit downloadBlock(C2CMD_DOWNLOAD_CONFIG, currentBlock_);
}

void DeviceConfig::unpack_() {
  numRows = eeprom_.matrixRows;
  numCols = eeprom_.matrixCols;
  numLayers = eeprom_.matrixLayers;
  uint8_t maxSwitchIndex = switchTypeNames_.size() - 1;
  switchType = std::min(eeprom_.switchType, maxSwitchIndex);
  setSwitchCapabilities_();
  memset(thresholds, EMPTY_FLASH_BYTE, sizeof(thresholds));
  memset(layouts, 0x00, sizeof(layouts));
  uint8_t tableSize = numRows * numCols;
  for (uint8_t i = 0; i < numRows; i++) {
    for (uint8_t j = 0; j < numCols; j++) {
      uint16_t offset = i * numCols + j;
      this->thresholds[i][j] =
          capabilities.hasThresholds ? eeprom_.stash[offset] : 1;
      for (uint8_t k = 0; k < numLayers; k++) {
        layouts[k][i][j] = eeprom_.stash[tableSize * (k + 1) + offset];
      }
    }
  }
  int macro_start = tableSize * (numLayers + 1);
  macros.clear();
  while(eeprom_.stash[macro_start] != 0xff) {
    size_t len = eeprom_.stash[macro_start + 2];
    if (len == 0) {
      // Most likely fresh ROM
      break;
    }
    macros.emplace_back(eeprom_.stash[macro_start],
                        eeprom_.stash[macro_start+1],
                        QByteArray(reinterpret_cast<const char *>(
                            eeprom_.stash+macro_start+3), len));
    macro_start += len + 3;
  }
  this->bValid = true;
  emit loaded();
  return;
}

void DeviceConfig::assemble_() {
  eeprom_.configVersion = 2;
  memset(eeprom_.stash, EMPTY_FLASH_BYTE, sizeof(eeprom_.stash));
  memset(eeprom_._RESERVED0, EMPTY_FLASH_BYTE, sizeof(eeprom_._RESERVED0));
  memset(eeprom_._RESERVED1, EMPTY_FLASH_BYTE, sizeof(eeprom_._RESERVED1));
  uint8_t tableSize = numRows * numCols;
  setSwitchCapabilities_();
  for (uint8_t i = 0; i < this->numRows; i++) {
    for (uint8_t j = 0; j < numCols; j++) {
      uint16_t offset = i * numCols + j;
      if (capabilities.hasThresholds) {
        eeprom_.stash[offset] = thresholds[i][j];
      }
      for (uint8_t k = 0; k < numLayers; k++) {
        this->eeprom_.stash[tableSize * (k + 1) + offset] =
            this->layouts[k][i][j];
      }
    }
  }

  size_t macros_cursor = tableSize * (numLayers + 1);
  for (auto& m : macros) {
    auto bin = m.toBin();
    for (uint8_t i=0; i<bin.length(); i++) {
      eeprom_.stash[macros_cursor++] = bin[i];
    }
  }

}

void DeviceConfig::fromFile() {
  QSettings settings;
  QFileDialog fd(nullptr, "Choose one file to import from");
  fd.setDirectory(settings.value(DEVICECONFIG_DIR_KEY).toString());
  fd.setNameFilter(tr("CommonSense config files(*.cfg)"));
  fd.setDefaultSuffix(QString("cfg"));
  fd.setFileMode(QFileDialog::ExistingFile);
  if (fd.exec()) {
    QStringList fns = fd.selectedFiles();
    QFile f(fns.at(0));
    f.open(QIODevice::ReadOnly);
    QDataStream ds(&f);
    ds.readRawData((char *)this->eeprom_.raw, sizeof(this->eeprom_.raw));
    qInfo() << "Imported config from" << fns.at(0);
    settings.setValue(DEVICECONFIG_DIR_KEY,
                      QFileInfo(fns.at(0)).canonicalPath());
    this->unpack_();
  }
}

void DeviceConfig::toFile() {
  QSettings settings;
  this->assemble_();
  QFileDialog fd(nullptr, "Choose one file to export to");
  fd.setDirectory(settings.value(DEVICECONFIG_DIR_KEY).toString());
  fd.setNameFilter(tr("CommonSense config files(*.cfg)"));
  fd.setDefaultSuffix(QString("cfg"));
  fd.setAcceptMode(QFileDialog::AcceptSave);
  if (fd.exec()) {
    QStringList fns = fd.selectedFiles();
    QFile f(fns.at(0));
    f.open(QIODevice::WriteOnly);
    QDataStream ds(&f);
    ds.writeRawData((const char *)this->eeprom_.raw, sizeof(this->eeprom_.raw));
    f.close();
    qInfo() << "Exported config to" << fns.at(0);
    settings.setValue(DEVICECONFIG_DIR_KEY,
                      QFileInfo(fns.at(0)).canonicalPath());
  }
}

void DeviceConfig::commit() {
  QMessageBox::StandardButton result = QMessageBox::question(
      nullptr, "Saving EEPROM!",
      "Do you want to write the config that is now in the device, to "
      "EEPROM?\n\nNOTICE\nIf thresholds don't fully take effect after "
      "commit\nPlease reset the device by using 'Revert' menu item!",
      QMessageBox::Yes | QMessageBox::No);
  if (result == QMessageBox::Yes)
    emit sendCommand(C2CMD_COMMIT, 1u);
}

void DeviceConfig::rollback() {
  QMessageBox::StandardButton result =
      QMessageBox::question(nullptr, "Resetting device!",
                            "Device will be reset, config will be restored "
                            "from EEPROM and downloaded to host. OK?",
                            QMessageBox::Yes | QMessageBox::No);
  if (result == QMessageBox::Yes)
    emit sendCommand(C2CMD_ROLLBACK, 1u);
}

std::vector<LayerCondition> DeviceConfig::loadLayers() {
  std::vector<LayerCondition> cnds(ABSOLUTE_MAX_LAYERS);
  for (uint8_t i = 0; i < ABSOLUTE_MAX_LAYERS; i++) {
    cnds[i] = LayerCondition(eeprom_.layerConditions[i]);
  }
  return cnds;
}

void DeviceConfig::setLayerCondition(int conditionIdx, LayerCondition cnd) {
  eeprom_.layerConditions[conditionIdx] = cnd.toBin();
}

void DeviceConfig::setLayers(std::vector<LayerCondition> lcs) {
  int numLCs = lcs.size();
  for (int i = 0; i < numLCs; i++)
    setLayerCondition(i, lcs[i]);
}

uint16_t DeviceConfig::getDelay(size_t pos) {
  return eeprom_.delayLib[pos];
}

void DeviceConfig::setDelay(size_t pos, uint16_t delay_ms) {
  eeprom_.delayLib[pos] = delay_ms;
}

HardwareConfig DeviceConfig::getHardwareConfig() {
  HardwareConfig retval;
  retval.adcBits = eeprom_.adcBits;
  retval.chargeDelay = eeprom_.chargeDelay;
  retval.dischargeDelay = eeprom_.dischargeDelay;
  retval.debouncingTicks = eeprom_.debouncingTicks;
  retval.expHdrMode = eeprom_.expMode;
  retval.expHdrParam1 = eeprom_.expParam1;
  retval.expHdrParam2 = eeprom_.expParam2;
  return retval;
}

void DeviceConfig::setHardwareConfig(HardwareConfig config) {
  eeprom_.adcBits = config.adcBits;
  eeprom_.chargeDelay = config.chargeDelay;
  eeprom_.dischargeDelay = config.dischargeDelay;
  eeprom_.debouncingTicks = config.debouncingTicks;
  eeprom_.expMode = config.expHdrMode;
  eeprom_.expParam1 = config.expHdrParam1;
  eeprom_.expParam2 = config.expHdrParam2;
}

const std::vector<std::string> DeviceConfig::getExpModeNames() {
  return expModeNames_;
}

const QString DeviceConfig::getSwitchTypeName() {
  return QString{switchTypeNames_.at(switchType).data()};
}

size_t DeviceConfig::getMatrixSize() const {
  return numRows * numCols;
}

void DeviceConfig::setSwitchCapabilities_() {
  capabilities.hasChargeSequencer = true;
  capabilities.hasTelemetry = true;
  capabilities.hasThresholds = true;
  capabilities.isNormallyLow = true;
  switch(switchType) {
    case SwitchType::ST_ADB:
    case SwitchType::ST_SUN:
    case SwitchType::ST_MICROSWITCH:
      capabilities.hasChargeSequencer = false;
      capabilities.hasTelemetry = false;
      capabilities.hasThresholds = false;
      break;
    case SwitchType::ST_MAGVALVE:
      capabilities.hasTelemetry = false;
      break;
    case SwitchType::ST_BEAMSPRING:
      capabilities.isNormallyLow = false;
      break;
    default:
      break;
  }
}

std::pair<uint8_t, uint8_t>
DeviceConfig::toRowCol(const uint8_t keyIndex) const {
 const uint8_t col = keyIndex % numCols;
 return {(keyIndex - col) / numCols, col};
}
