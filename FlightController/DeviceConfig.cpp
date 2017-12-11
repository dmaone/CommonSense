#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

#include "DeviceConfig.h"
#include "LayerCondition.h"
#include "settings.h"

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
  if (transferDirection != TransferIdle) {
    qInfo() << "Not a good day to upload config!";
    return;
  }
  this->_assemble();
  this->transferDirection = TransferUpload;
  this->currentBlock = 0;
  qInfo() << "Uploading config..";
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
  }
}

void DeviceConfig::fromDevice() {
  switch (transferDirection) {
  case TransferIdle:
    transferDirection = TransferDownload;
    currentBlock = 0;
    qInfo() << "Downloading config..";
    break;
  case TransferDownload:
    qInfo() << "Already downloading! Re-requesting current block just in case";
    break;
  default:
    qInfo() << "Not a good day to download config!";
    return;
  }
  emit(downloadBlock(C2CMD_DOWNLOAD_CONFIG, currentBlock));
}

/**
 * @brief DeviceInterface::_downloadConfigBlock
 * Receives one block from device, writes it to local config.
 * @param payload - packet payload
 */
void DeviceConfig::_receiveConfigBlock(QByteArray *payload) {
  if (transferDirection != TransferDownload) {
    qInfo() << "Not a good day to download config block!";
    return;
  }
  if (currentBlock >= (EEPROM_BYTESIZE / CONFIG_TRANSFER_BLOCK_SIZE)) {
    transferDirection = TransferIdle;
    qInfo() << "done, unpacking...";
    _unpack();
    return;
  }
  qInfo(".");
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
  // uint8_t macros_start = tableSize * (numLayers + 1);
  /*
  // Crude memcpy of a test macro F24 -> Shift-A
  static const uint8_t filler[] = {
      115, 0, 6, // F24, default flags, 6 bytes
      0x40, 0xe1, // Press, LShift
      0x00, 4, // Type, A
      0x60, 0xe1, // Release, LShift
      229, 0xc0, 6, // RShift, release|tap, 6 bytes
      0x40, 0xe1, // Press, LShift
      00, 5, // Type, B
      0x60, 0xe1, // Release, LShift
      0xff // last byte, so that I can put comma at the end of the block
  };
  for (uint8_t i = 0; i < sizeof filler; i++)
  {
      this->_eeprom.stash[macros_start + i] = filler[i];
  }
  */
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

std::vector<LayerCondition> DeviceConfig::layerConditions(void) {
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

std::vector<uint8_t> DeviceConfig::expHeaderParams(void) {
  std::vector<uint8_t> retval;
  retval.push_back(_eeprom.expMode);
  retval.push_back(_eeprom.expParam1);
  retval.push_back(_eeprom.expParam2);
  return retval;
}

void DeviceConfig::setExpHeaderParams(uint8_t mode, uint8_t param1,
                                      uint8_t param2) {
  _eeprom.expMode = mode;
  _eeprom.expParam1 = param1;
  _eeprom.expParam2 = param2;
}
