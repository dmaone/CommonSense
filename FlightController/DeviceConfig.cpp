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
  memset(this->_eeprom.raw, 0x00, sizeof(this->_eeprom));
}

bool DeviceConfig::eventFilter(QObject* /* obj */, QEvent *event) {
  if (event->type() != DeviceMessage::ET) {
    return false;
  }
  QByteArray *payload = static_cast<DeviceMessage *>(event)->getPayload();
  if (payload->at(0) != C2RESPONSE_CONFIG)
    return false;

  currentBlock++;
  switch (transferDirection_) {
  case TransferDownload:
    _receiveConfigBlock(payload);
    break;
  case TransferUpload:
    _uploadConfigBlock();
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
  if (interface_->getStatusBit(C2DEVSTATUS_MATRIX_MONITOR)) {
    QMessageBox::critical(nullptr, "Matrix monitor active",
                          "Turn off matrix monitor first!");
    return;
  }
  if (transferDirection_ != TransferIdle) {
    qInfo() << "Not a good day to upload config!";
    QMessageBox::critical(nullptr, "Not a good day to upload config",
                          "Error! Try pressing 'Reconnect' button!");
    return;
  }
  this->_assemble();
  emit sendCommand(C2CMD_EWO, (1 << C2DEVSTATUS_SETUP_MODE));
  this->transferDirection_ = TransferUpload;
  this->currentBlock = 0;
  qInfo() << "Uploading config";
  this->_uploadConfigBlock();
}

/**
 * @brief DeviceInterface::uploadConfigBlock
 * Upload one block to device.
 * We don't really care about the input packet here
 */
void DeviceConfig::_uploadConfigBlock() {
  switch (transferDirection_) {
  case TransferUpload:
    if (currentBlock > (EEPROM_BYTESIZE / CONFIG_TRANSFER_BLOCK_SIZE)) {
      qInfo() << "done!";
      transferDirection_ = TransferIdle;
      emit sendCommand(C2CMD_APPLY_CONFIG, 1);
      return;
    }
    qInfo(".");
    OUT_c2packet_t msg;
    msg.command = C2CMD_UPLOAD_CONFIG;
    msg.payload[0] = currentBlock;
    memcpy(msg.payload + CONFIG_BLOCK_DATA_OFFSET,
           this->_eeprom.raw + (CONFIG_TRANSFER_BLOCK_SIZE * currentBlock),
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
  if (interface_->getStatusBit(C2DEVSTATUS_MATRIX_MONITOR)) {
    QMessageBox::critical(nullptr, "Matrix monitor active",
                          "Turn off matrix monitor first!");
    return;
  }
  switch (transferDirection_) {
  case TransferIdle:
    transferDirection_ = TransferDownload;
    currentBlock = 0;
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
  emit downloadBlock(C2CMD_DOWNLOAD_CONFIG, currentBlock);
}

/**
 * @brief DeviceInterface::_downloadConfigBlock
 * Receives one block from device, writes it to local config.F
 * @param payload - packet payload
 */
void DeviceConfig::_receiveConfigBlock(QByteArray *payload) {
  if (transferDirection_ != TransferDownload) {
    qInfo() << "Not a good day to download config block!";
    QMessageBox::critical(nullptr, "Not a good day to download config blockb",
                          "Error! Try pressing 'Reconnect' button!");
    return;
  }
  qInfo(".");
  if (currentBlock >= (EEPROM_BYTESIZE / CONFIG_TRANSFER_BLOCK_SIZE)) {
    transferDirection_ = TransferIdle;
    qInfo() << "done, unpacking...";
    _unpack();
    return;
  }
  memcpy(this->_eeprom.raw +
             (CONFIG_TRANSFER_BLOCK_SIZE * (uint8_t)payload->at(1)),
         payload->data() + 1 + CONFIG_BLOCK_DATA_OFFSET,
         CONFIG_TRANSFER_BLOCK_SIZE);
  emit downloadBlock(C2CMD_DOWNLOAD_CONFIG, currentBlock);
}

void DeviceConfig::_unpack() {
  numRows = _eeprom.matrixRows;
  numCols = _eeprom.matrixCols;
  numLayers = _eeprom.matrixLayers;
  uint8_t maxSwitchIndex = switchTypeNames_.size() - 1;
  switchType = std::min(_eeprom.switchType, maxSwitchIndex);
  _setSwitchCapabilities();
  memset(thresholds, EMPTY_FLASH_BYTE, sizeof(thresholds));
  memset(layouts, 0x00, sizeof(layouts));
  uint8_t tableSize = numRows * numCols;
  for (uint8_t i = 0; i < numRows; i++) {
    for (uint8_t j = 0; j < numCols; j++) {
      uint16_t offset = i * numCols + j;
      this->thresholds[i][j] =
          capabilities.hasThresholds ? _eeprom.stash[offset] : 1;
      for (uint8_t k = 0; k < numLayers; k++) {
        layouts[k][i][j] = _eeprom.stash[tableSize * (k + 1) + offset];
      }
    }
  }
  int macro_start = tableSize * (numLayers + 1);
  macros.clear();
  while(_eeprom.stash[macro_start] != 0xff) {
    size_t len = _eeprom.stash[macro_start + 2];
    if (len == 0) {
      // Most likely fresh ROM
      break;
    }
    macros.emplace_back(_eeprom.stash[macro_start],
                        _eeprom.stash[macro_start+1],
                        QByteArray(reinterpret_cast<const char *>(
                            _eeprom.stash+macro_start+3), len));
    macro_start += len + 3;
  }
  this->bValid = true;
  emit changed();
  return;
}

void DeviceConfig::_assemble() {
  _eeprom.configVersion = 2;
  memset(_eeprom.stash, EMPTY_FLASH_BYTE, sizeof(_eeprom.stash));
  memset(_eeprom._RESERVED0, EMPTY_FLASH_BYTE, sizeof(_eeprom._RESERVED0));
  memset(_eeprom._RESERVED1, EMPTY_FLASH_BYTE, sizeof(_eeprom._RESERVED1));
  uint8_t tableSize = numRows * numCols;
  _setSwitchCapabilities();
  for (uint8_t i = 0; i < this->numRows; i++) {
    for (uint8_t j = 0; j < numCols; j++) {
      uint16_t offset = i * numCols + j;
      if (capabilities.hasThresholds) {
        _eeprom.stash[offset] = thresholds[i][j];
      }
      for (uint8_t k = 0; k < numLayers; k++) {
        this->_eeprom.stash[tableSize * (k + 1) + offset] =
            this->layouts[k][i][j];
      }
    }
  }

  size_t macros_cursor = tableSize * (numLayers + 1);
  for (auto& m : macros) {
    auto bin = m.toBin();
    for (uint8_t i=0; i<bin.length(); i++) {
      _eeprom.stash[macros_cursor++] = bin[i];
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
    ds.readRawData((char *)this->_eeprom.raw, sizeof(this->_eeprom.raw));
    qInfo() << "Imported config from" << fns.at(0);
    settings.setValue(DEVICECONFIG_DIR_KEY,
                      QFileInfo(fns.at(0)).canonicalPath());
    this->_unpack();
  }
}

void DeviceConfig::toFile() {
  QSettings settings;
  this->_assemble();
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
    ds.writeRawData((const char *)this->_eeprom.raw, sizeof(this->_eeprom.raw));
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
    cnds[i] = LayerCondition(_eeprom.layerConditions[i]);
  }
  return cnds;
}

void DeviceConfig::setLayerCondition(int conditionIdx, LayerCondition cnd) {
  _eeprom.layerConditions[conditionIdx] = cnd.toBin();
}

void DeviceConfig::setLayers(std::vector<LayerCondition> lcs) {
  int numLCs = lcs.size();
  for (int i = 0; i < numLCs; i++)
    setLayerCondition(i, lcs[i]);
}

uint16_t DeviceConfig::getDelay(size_t pos) {
  return _eeprom.delayLib[pos];
}

void DeviceConfig::setDelay(size_t pos, uint16_t delay_ms) {
  _eeprom.delayLib[pos] = delay_ms;
}

HardwareConfig DeviceConfig::getHardwareConfig() {
  HardwareConfig retval;
  retval.adcBits = _eeprom.adcBits;
  retval.chargeDelay = _eeprom.chargeDelay;
  retval.dischargeDelay = _eeprom.dischargeDelay;
  retval.debouncingTicks = _eeprom.debouncingTicks;
  retval.expHdrMode = _eeprom.expMode;
  retval.expHdrParam1 = _eeprom.expParam1;
  retval.expHdrParam2 = _eeprom.expParam2;
  return retval;
}

void DeviceConfig::setHardwareConfig(HardwareConfig config) {
  _eeprom.adcBits = config.adcBits;
  _eeprom.chargeDelay = config.chargeDelay;
  _eeprom.dischargeDelay = config.dischargeDelay;
  _eeprom.debouncingTicks = config.debouncingTicks;
  _eeprom.expMode = config.expHdrMode;
  _eeprom.expParam1 = config.expHdrParam1;
  _eeprom.expParam2 = config.expHdrParam2;
}

const std::vector<std::string> DeviceConfig::getExpModeNames() {
  return expModeNames_;
}

const std::string& DeviceConfig::getSwitchTypeName() {
  return switchTypeNames_.at(switchType);
}

void DeviceConfig::_setSwitchCapabilities() {
  capabilities.hasChargeSequencer = true;
  capabilities.hasMatrixMonitor = true;
  capabilities.hasThresholds = true;
  capabilities.isNormallyLow = true;
  switch(switchType) {
    case SwitchType::ST_ADB:
    case SwitchType::ST_SUN:
    case SwitchType::ST_MICROSWITCH:
      capabilities.hasChargeSequencer = false;
      capabilities.hasMatrixMonitor = false;
      capabilities.hasThresholds = false;
      break;
    case SwitchType::ST_MAGVALVE:
      capabilities.hasMatrixMonitor = false;
      break;
    case SwitchType::ST_BEAMSPRING:
      capabilities.isNormallyLow = false;
      break;
    default:
      break;
  }
}
