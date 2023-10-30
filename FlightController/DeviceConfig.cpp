#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

#include "DeviceConfig.h"
#include "DeviceInterface.h"
#include "Events.h"
#include "LayerCondition.h"
#include "settings.h"

namespace {

constexpr size_t fwVer(uint8_t major, uint8_t minor) {
  return (major << 8) + minor;
}

static constexpr uint8_t kMinFwMajor{1};
static constexpr uint8_t kMinFwMinor{4};
static constexpr auto kMinFw{fwVer(kMinFwMajor, kMinFwMinor)};

} //namespace

DeviceConfig::DeviceConfig(DeviceInterface* di) : interface_{di} {
  reset(0);
}

void DeviceConfig::reset(uint8_t mtu) {
  memset(this->eeprom_.raw, 0x00, sizeof(this->eeprom_));
  bValid = false;
  transferDirection_ = TransferIdle;
  currentBlock_ = 0;
  blockSize_ = mtu > 65 ? CONFIG_TRANSFER_SIZE_BLE : CONFIG_TRANSFER_SIZE_USB;
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
  if (fwVer(interface_->firmwareMajor, interface_->firmwareMinor) < kMinFw) {
    auto errMsg = QString("Please update firmware to v%1.%2 or newer!")
        .arg(kMinFwMajor).arg(kMinFwMinor);
    QMessageBox::critical(nullptr, "Firmware too old", errMsg);
    return;
  }
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
    if (currentBlock_ >= (EEPROM_BYTESIZE / blockSize_)) {
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
           this->eeprom_.raw + (blockSize_ * currentBlock_),
           blockSize_);
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
  if (currentBlock_ >= (EEPROM_BYTESIZE / blockSize_)) {
    transferDirection_ = TransferIdle;
    qInfo() << "done, unpacking...";
    unpack_();
    return;
  }
  memcpy(this->eeprom_.raw + (blockSize_ * (uint8_t)payload->at(1)),
         payload->data() + 1 + CONFIG_BLOCK_DATA_OFFSET,
         blockSize_);
  emit downloadBlock(C2CMD_DOWNLOAD_CONFIG, currentBlock_);
}

uint16_t DeviceConfig::getLayoutOffset_(
    uint8_t eepromVersion, uint8_t row, uint8_t col, uint8_t layer) {
  if (eepromVersion < 3) {
    return getMatrixSize() * (layer + 1) + row * numCols + col;
  }
  return getMatrixSize() + (row * numCols + col) * numLayers + layer;
}

void DeviceConfig::unpack_() {
  if (eeprom_.configVersion < CS_LAST_COMPATIBLE_NVRAM_VERSION)  {
    qWarning().nospace().noquote() << "EEPROM layout is too old (version "
        << eeprom_.configVersion << ")! Minimum supported version is "
        << CS_LAST_COMPATIBLE_NVRAM_VERSION << ".";
  }
  numRows = eeprom_.matrixRows;
  numCols = eeprom_.matrixCols;
  numPedals = eeprom_.pedals == EMPTY_FLASH_BYTE ? 0 : eeprom_.pedals;
  numLayers = eeprom_.matrixLayers;
  uint8_t maxSwitchIndex = switchTypeNames_.size() - 1;
  switchType = std::min(eeprom_.switchType, maxSwitchIndex);
  setSwitchCapabilities_();
  if (eeprom_.hostMode == EMPTY_FLASH_BYTE) {
    eeprom_.hostMode = HM_WINDOWS;
  }
  memset(thresholds, EMPTY_FLASH_BYTE, sizeof(thresholds));
  memset(layouts, 0x00, sizeof(layouts));
  uint16_t curPos{0};
  for (uint8_t r = 0; r < numRows; r++) {
    for (uint8_t c = 0; c < numCols; c++) {
      thresholds[r][c] = capabilities.hasThresholds ? eeprom_.stash[curPos]
                                                    : EMPTY_FLASH_BYTE;
      curPos++;
    }
  }

  auto zeroEmpty = [this] (uint16_t idx) {
    return eeprom_.stash[idx] == EMPTY_FLASH_BYTE ? 0 : eeprom_.stash[idx];
  };

  for (uint8_t r = 0; r < numRows; r++) {
    for (uint8_t c = 0; c < numCols; c++) {
      for (uint8_t l = 0; l < numLayers; l++) {
        layouts[l][r][c] =
            zeroEmpty(getLayoutOffset_(eeprom_.configVersion, r, c, l));
        curPos++; // Adjusting position - direct addressing is used here..
      }
    }
  }

  memset(pedals, 0x00, sizeof(pedals));
  memset(pedalFlags, 0x00, sizeof(pedalFlags));
  for (uint8_t p = 0; p < numPedals; p++) {
    for (uint8_t l = 0; l < numLayers; l++) {
      pedals[p][l] = zeroEmpty(curPos++);
    }
  }
  for (uint8_t p = 0; p < numPedals; p++) {
    pedalFlags[p] = eeprom_.stash[curPos++];
  }

  macros.clear();
  while(eeprom_.stash[curPos] != EMPTY_FLASH_BYTE) {
    size_t len = eeprom_.stash[curPos + 2];
    if (len == 0) {
      // Most likely fresh ROM
      break;
    }
    macros.emplace_back(eeprom_.stash[curPos],
                        eeprom_.stash[curPos+1],
                        QByteArray(reinterpret_cast<const char *>(
                            eeprom_.stash+curPos+3), len));
    curPos += len + 3;
  }
  this->bValid = true;
  emit loaded();
  return;
}

void DeviceConfig::assemble_() {
  eeprom_.configVersion = CS_CONFIG_VERSION;
  memset(eeprom_.stash, EMPTY_FLASH_BYTE, sizeof(eeprom_.stash));
  memset(eeprom_._RESERVED0, EMPTY_FLASH_BYTE, sizeof(eeprom_._RESERVED0));
  memset(eeprom_._RESERVED1, EMPTY_FLASH_BYTE, sizeof(eeprom_._RESERVED1));
  setSwitchCapabilities_();
  uint16_t curPos{0};
  for (uint8_t r = 0; r < numRows; r++) {
    for (uint8_t c = 0; c < numCols; c++) {
      if (capabilities.hasThresholds) {
        eeprom_.stash[curPos] = thresholds[r][c];
      }
      curPos++;
    }
  }
  for (uint8_t r = 0; r < numRows; r++) {
    for (uint8_t c = 0; c < numCols; c++) {
      for (uint8_t l = 0; l < numLayers; l++) {
        eeprom_.stash[curPos++] = this->layouts[l][r][c];
      }
    }
  }
  for (uint8_t p = 0; p < numPedals; p++) {
    for (uint8_t l = 0; l < numLayers; l++) {
      eeprom_.stash[curPos++] = pedals[p][l];
    }
  }
  for (uint8_t p = 0; p < numPedals; p++) {
    eeprom_.stash[curPos++] = pedalFlags[p];
  }
  for (auto& m : macros) {
    auto bin = m.toBin();
    for (uint8_t i=0; i < bin.length(); i++) {
      eeprom_.stash[curPos++] = bin[i];
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
  return HardwareConfig{
    .adcBits = eeprom_.adcBits,
    .chargeDelay = eeprom_.chargeDelay,
    .dischargeDelay = eeprom_.dischargeDelay,
    .debouncingTicks = eeprom_.debouncingTicks,
    .pedalDebouncingTicks = eeprom_.pedalDebouncingTicks,
    .expHdrMode = eeprom_.expMode,
    .expHdrParam1 = eeprom_.expParam1,
    .expHdrParam2 = eeprom_.expParam2,
    .hostMode = eeprom_.hostMode,
  };
}

void DeviceConfig::setHardwareConfig(HardwareConfig config) {
  eeprom_.adcBits = config.adcBits;
  eeprom_.chargeDelay = config.chargeDelay;
  eeprom_.dischargeDelay = config.dischargeDelay;
  eeprom_.debouncingTicks = config.debouncingTicks;
  eeprom_.pedalDebouncingTicks = config.pedalDebouncingTicks;
  eeprom_.expMode = config.expHdrMode;
  eeprom_.expParam1 = config.expHdrParam1;
  eeprom_.expParam2 = config.expHdrParam2;
  eeprom_.hostMode = config.hostMode;
}

const std::vector<std::string> DeviceConfig::getExpModeNames() {
  return expModeNames_;
}

const std::vector<std::string> DeviceConfig::getHostModeNames() {
  return hostModeNames_;
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
    case SwitchType::ST_CONTACT:
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
