#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

#include "DeviceConfig.h"
#include "DeviceInterface.h"
#include "LayerCondition.h"
#include "settings.h"
#include "singleton.h"

DeviceConfig::DeviceConfig(QObject *parent)
    : QObject(parent), bValid(false), numRows(0), numCols(0),
      numLayers(ABSOLUTE_MAX_LAYERS), numLayerConditions(NUM_LAYER_CONDITIONS),
      numDelays(NUM_DELAYS), bNormallyLow(false),
      transferDirection(TransferIdle) {
  memset(this->_eeprom.raw, 0x00, sizeof(this->_eeprom));
}

bool DeviceConfig::eventFilter(QObject *obj __attribute__((unused)),
                               QEvent *event) {
  if (event->type() != DeviceMessage::ET)
    return false;
  QByteArray *payload = static_cast<DeviceMessage *>(event)->getPayload();
  if (payload->at(0) != C2RESPONSE_CONFIG)
    return false;

  currentBlock++;
  switch (transferDirection) {
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
void DeviceConfig::toDevice(void) {
  DeviceInterface &di = Singleton<DeviceInterface>::instance();
  if (di.getStatusBit(C2DEVSTATUS_MATRIX_MONITOR)) {
    QMessageBox::critical(NULL, "Matrix monitor active",
                          "Turn off matrix monitor first!");
    return;
  }
  if (transferDirection != TransferIdle) {
    qInfo() << "Not a good day to upload config!";
    QMessageBox::critical(NULL, "Not a good day to upload config",
                          "Error! Try pressing 'Reconnect' button!");
    return;
  }
  this->_assemble();
  emit sendCommand(C2CMD_EWO, (1 << C2DEVSTATUS_SETUP_MODE));
  this->transferDirection = TransferUpload;
  this->currentBlock = 0;
  qInfo() << "Uploading config";
  this->_uploadConfigBlock();
}

/**
 * @brief DeviceInterface::uploadConfigBlock
 * Upload one block to device.
 * We don't really care about the input packet here, hence (void)
 */
void DeviceConfig::_uploadConfigBlock(void) {
  switch (transferDirection) {
  case TransferUpload:
    if (currentBlock > (EEPROM_BYTESIZE / CONFIG_TRANSFER_BLOCK_SIZE)) {
      qInfo() << "done!";
      transferDirection = TransferIdle;
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
    emit(uploadBlock(msg));
    break;
  default:
    qInfo() << "Not a good day to upload config block!";
    QMessageBox::critical(NULL, "Not a good day to upload config block",
                          "Error! Try pressing 'Reconnect' button!");
  }
}

void DeviceConfig::fromDevice() {
  DeviceInterface &di = Singleton<DeviceInterface>::instance();
  if (di.getStatusBit(C2DEVSTATUS_MATRIX_MONITOR)) {
    QMessageBox::critical(NULL, "Matrix monitor active",
                          "Turn off matrix monitor first!");
    return;
  }
  switch (transferDirection) {
  case TransferIdle:
    transferDirection = TransferDownload;
    currentBlock = 0;
    qInfo() << "Downloading config";
    qInfo() << ".";
    break;
  case TransferDownload:
    qInfo() << "Already downloading! Re-requesting current block just in case";
    break;
  default:
    qInfo() << "Not a good day to download config!";
    QMessageBox::critical(NULL, "Not a good day to download config",
                          "Error! Try pressing 'Reconnect' button!");
    return;
  }
  emit(downloadBlock(C2CMD_DOWNLOAD_CONFIG, currentBlock));
}

/**
 * @brief DeviceInterface::_downloadConfigBlock
 * Receives one block from device, writes it to local config.F
 * @param payload - packet payload
 */
void DeviceConfig::_receiveConfigBlock(QByteArray *payload) {
  if (transferDirection != TransferDownload) {
    qInfo() << "Not a good day to download config block!";
    QMessageBox::critical(NULL, "Not a good day to download config blockb",
                          "Error! Try pressing 'Reconnect' button!");
    return;
  }
  qInfo(".");
  if (currentBlock >= (EEPROM_BYTESIZE / CONFIG_TRANSFER_BLOCK_SIZE)) {
    transferDirection = TransferIdle;
    qInfo() << "done, unpacking...";
    _unpack();
    return;
  }
  memcpy(this->_eeprom.raw +
             (CONFIG_TRANSFER_BLOCK_SIZE * (uint8_t)payload->at(1)),
         payload->data() + 1 + CONFIG_BLOCK_DATA_OFFSET,
         CONFIG_TRANSFER_BLOCK_SIZE);
  emit(downloadBlock(C2CMD_DOWNLOAD_CONFIG, currentBlock));
}

void DeviceConfig::_unpack(void) {
  numRows = _eeprom.matrixRows;
  numCols = _eeprom.matrixCols;
  numLayers = _eeprom.matrixLayers;
  bNormallyLow = _eeprom.capsenseFlags & (1 << CSF_NL);
  switchType = std::min(_eeprom.switchType, switchTypeCount);
  memset(thresholds, EMPTY_FLASH_BYTE, sizeof(thresholds));
  memset(layouts, 0x00, sizeof(layouts));
  uint8_t tableSize = numRows * numCols;
  for (uint8_t i = 0; i < numRows; i++) {
    for (uint8_t j = 0; j < numCols; j++) {
      uint16_t offset = i * numCols + j;
      this->thresholds[i][j] = _eeprom.stash[offset];
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
  emit(changed());
  return;
}

void DeviceConfig::_assemble(void) {
  _eeprom.configVersion = 2;
  memset(_eeprom.stash, EMPTY_FLASH_BYTE, sizeof(_eeprom.stash));
  memset(_eeprom._RESERVED0, EMPTY_FLASH_BYTE, sizeof(_eeprom._RESERVED0));
  memset(_eeprom._RESERVED1, EMPTY_FLASH_BYTE, sizeof(_eeprom._RESERVED1));
  uint8_t tableSize = numRows * numCols;
  for (uint8_t i = 0; i < this->numRows; i++) {
    for (uint8_t j = 0; j < numCols; j++) {
      uint16_t offset = i * numCols + j;
      _eeprom.stash[offset] = thresholds[i][j];
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
  QFileDialog fd(Q_NULLPTR, "Choose one file to import from");
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
  QFileDialog fd(Q_NULLPTR, "Choose one file to export to");
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

void DeviceConfig::commit(void) {
  QMessageBox::StandardButton result = QMessageBox::question(
      NULL, "Saving EEPROM!",
      "Do you want to write the config that is now in the device, to "
      "EEPROM?\n\nNOTICE\nIf thresholds don't fully take effect after "
      "commit\nPlease reset the device by using 'Revert' menu item!",
      QMessageBox::Yes | QMessageBox::No);
  if (result == QMessageBox::Yes)
    emit sendCommand(C2CMD_COMMIT, 1u);
}

void DeviceConfig::rollback(void) {
  QMessageBox::StandardButton result =
      QMessageBox::question(NULL, "Resetting device!",
                            "Device will be reset, config will be restored "
                            "from EEPROM and downloaded to host. OK?",
                            QMessageBox::Yes | QMessageBox::No);
  if (result == QMessageBox::Yes)
    emit sendCommand(C2CMD_ROLLBACK, 1u);
}

std::vector<LayerCondition> DeviceConfig::loadLayerConditions(void) {
  std::vector<LayerCondition> cnds(numLayerConditions);
  for (uint8_t i = 0; i < numLayerConditions; i++) {
    cnds[i] = LayerCondition(_eeprom.layerConditions[i]);
  }
  return cnds;
}

void DeviceConfig::setLayerCondition(int conditionIdx, LayerCondition cnd) {
  _eeprom.layerConditions[conditionIdx] = cnd.toBin();
}

void DeviceConfig::setLayerConditions(std::vector<LayerCondition> lcs) {
  int numLCs = lcs.size();
  for (int i = 0; i < numLCs; i++)
    setLayerCondition(i, lcs[i]);
}

std::vector<uint16_t> DeviceConfig::delays(void) {
  std::vector<uint16_t> retval(numDelays);
  for (uint8_t i = 0; i < numDelays; i++) {
    retval[i] = _eeprom.delayLib[i];
  }
  return retval;
}

void DeviceConfig::setDelay(int delayIdx, uint16_t delay_ms) {
  _eeprom.delayLib[delayIdx] = delay_ms;
}

HardwareConfig DeviceConfig::getHardwareConfig(void) {
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
