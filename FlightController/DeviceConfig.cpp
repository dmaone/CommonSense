#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

#include "settings.h"
#include "DeviceConfig.h"

using namespace std;

LayerCondition::LayerCondition(void):
    fnKeys(0),
    layer_(0)
{
}

LayerCondition::LayerCondition(unsigned char bin)
{
    fnKeys = (bin >> 4);
    if (fnKeys > 15)
        fnKeys = 0;

    layer_ = bin & 0x0f;
}

LayerCondition::LayerCondition(bool fn1, bool fn2, bool fn3, bool fn4, int layer):
    fnKeys(0),
    layer_(layer)
{
    if (fn1)
        fnKeys |= fkmFn1;
    if (fn2)
        fnKeys |= fkmFn2;
    if (fn3)
        fnKeys |= fkmFn3;
    if (fn4)
        fnKeys |= fkmFn4;
}

bool LayerCondition::fn1Set(void)
{
    return (fnKeys & fkmFn1) > 0;
}

bool LayerCondition::fn2Set(void)
{
    return (fnKeys & fkmFn2) > 0;
}

bool LayerCondition::fn3Set(void)
{
    return (fnKeys & fkmFn3) > 0;
}

bool LayerCondition::fn4Set(void)
{
    return (fnKeys & fkmFn4) > 0;
}

int LayerCondition::layer(void)
{
    return layer_;
}

unsigned char LayerCondition::toBin(void)
{
    return (fnKeys << 4) | (layer_ & 0xf);
}

DeviceConfig::DeviceConfig(QObject *parent) : QObject(parent),
    bValid(false), numRows(0), numCols(0), numLayers(MAX_LAYERS),
    numLayerConditions(MAX_LAYER_CONDITIONS),bNormallyLow(false),
    transferDirection(TransferIdle)
{
    memset(this->_eeprom.raw, 0x00, sizeof(this->_eeprom));
}

bool DeviceConfig::eventFilter(QObject *obj __attribute__((unused)), QEvent *event){
    if (event->type() != DeviceMessage::ET )
        return false;
    QByteArray *payload = static_cast<DeviceMessage *>(event)->getPayload();
    if (payload->at(0) != C2RESPONSE_CONFIG)
        return false;

    currentBlock++;
    switch (transferDirection)
    {
        case TransferDownload:
            _receiveConfigBlock(payload);
            break;
        case TransferUpload:
            _uploadConfigBlock();
            break;
        default:
            qInfo() << "Received config block" << ((uint8_t)payload->at(1)) << "while supposed to be idle!";
    }
    return true;
}

/**
 * @brief DeviceInterface::uploadConfig
 * Fire up the uploader.
 */
void DeviceConfig::toDevice(void)
{
    if ( transferDirection != TransferIdle )
    {
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
void DeviceConfig::_uploadConfigBlock(void)
{
    switch(transferDirection)
    {
        case TransferUpload:
            if (currentBlock > (EEPROM_BYTESIZE / CONFIG_TRANSFER_BLOCK_SIZE))
            {
                qInfo() << "done!";
                transferDirection = TransferIdle;
                return;
            }
            qInfo(".");
            OUT_c2packet_t msg;
            msg.command = C2CMD_UPLOAD_CONFIG;
            msg.payload[0] = currentBlock;
            memcpy(
                msg.payload + CONFIG_BLOCK_DATA_OFFSET,
                this->_eeprom.raw+(CONFIG_TRANSFER_BLOCK_SIZE * currentBlock),
                CONFIG_TRANSFER_BLOCK_SIZE
            );
            emit(uploadBlock(msg));
            break;
        default:
            qInfo() << "Not a good day to upload config block!";
    }
}

void DeviceConfig::fromDevice()
{
    switch(transferDirection)
    {
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
void DeviceConfig::_receiveConfigBlock(QByteArray *payload)
{
    if ( transferDirection != TransferDownload )
    {
        qInfo() << "Not a good day to download config block!";
        return;
    }
    if (currentBlock >= (EEPROM_BYTESIZE / CONFIG_TRANSFER_BLOCK_SIZE))
    {
        transferDirection = TransferIdle;
        qInfo() << "done, unpacking...";
        _unpack();
        return;
    }
    qInfo(".");
    memcpy(
        this->_eeprom.raw+(CONFIG_TRANSFER_BLOCK_SIZE * (uint8_t)payload->at(1)),
        payload->data() + 1 + CONFIG_BLOCK_DATA_OFFSET,
        CONFIG_TRANSFER_BLOCK_SIZE
    );
    emit(downloadBlock(C2CMD_DOWNLOAD_CONFIG, currentBlock));
}

void DeviceConfig::_unpack(void)
{
    numRows   = _eeprom.matrixRows;
    numCols   = _eeprom.matrixCols;
    numLayers = _eeprom.layerCount;
    bNormallyLow = _eeprom.capsenseFlags & (1 << CSF_NL);
    guardLo   = _eeprom.guardLo;
    guardHi   = _eeprom.guardHi;
    memset(deadBandLo, 0xfe, sizeof(deadBandLo));
    memset(deadBandHi, 0xfe, sizeof(deadBandHi));
    memset(layouts, 0x00, sizeof(layouts));
    uint8_t table_size = numRows * numCols;
    for (uint8_t i = 0; i < numRows; i++)
    {
        for (uint8_t j = 0; j < numCols; j++)
        {
            uint16_t offset = i*numCols + j;
            this->deadBandLo[i][j] = _eeprom.stash[offset];
            this->deadBandHi[i][j] = _eeprom.stash[table_size + offset];
            this->skipSensing[i][j] = (deadBandLo[i][j] > deadBandHi[i][j]);
            for (uint8_t k = 0; k < numLayers; k++)
            {
                layouts[k][i][j] = _eeprom.stash[table_size*(k+2) + offset];
            }
        }
    }
    this->bValid = true;
    emit(changed());
    return;
}

void DeviceConfig::_assemble(void)
{
    _eeprom.configVersion = 2;
    _eeprom.guardLo = guardLo;
    _eeprom.guardHi = guardHi;
    _eeprom.layerCount = numLayers;
    memset(_eeprom.stash, 0, sizeof(_eeprom.stash));
    memset(_eeprom._RESERVED0, 0xff, sizeof(_eeprom._RESERVED0));
    memset(_eeprom._RESERVED1, 0xff, sizeof(_eeprom._RESERVED1));
    uint8_t table_size = numRows * numCols;
    for (uint8_t i = 0; i < this->numRows; i++)
    {
        for (uint8_t j = 0; j < numCols; j++)
        {
            uint16_t offset = i*numCols + j;
// TODO make it easier on the script|more visible on UI.
            if (deadBandHi[i][j] < deadBandLo[i][j])
            {
                deadBandLo[i][j] = 255;
                deadBandHi[i][j] = 0;
            }
            _eeprom.stash[offset] = deadBandLo[i][j];
            _eeprom.stash[table_size + offset] = deadBandHi[i][j];
            for (uint8_t k = 0; k < numLayers; k++)
            {
                this->_eeprom.stash[table_size*(k+2) + offset] = this->layouts[k][i][j];
            }
        }
    }
}

void DeviceConfig::fromFile()
{
    QSettings settings;
    QFileDialog fd(Q_NULLPTR, "Choose one file to import from");
    fd.setDirectory(settings.value(DEVICECONFIG_DIR_KEY).toString());
    fd.setNameFilter(tr("CommonSense config files(*.cfg)"));
    fd.setDefaultSuffix(QString("cfg"));
    fd.setFileMode(QFileDialog::ExistingFile);
    if (fd.exec())
    {
        QStringList fns = fd.selectedFiles();
        QFile f(fns.at(0));
        f.open(QIODevice::ReadOnly);
        QDataStream ds(&f);
        ds.readRawData((char *)this->_eeprom.raw, sizeof(this->_eeprom.raw));
        qInfo() << "Imported config from" << fns.at(0);
        settings.setValue(DEVICECONFIG_DIR_KEY, QFileInfo(fns.at(0)).canonicalPath());
        this->_unpack();
    }
}

void DeviceConfig::toFile()
{
    QSettings settings;
    this->_assemble();
    QFileDialog fd(Q_NULLPTR, "Choose one file to export to");
    fd.setDirectory(settings.value(DEVICECONFIG_DIR_KEY).toString());
    fd.setNameFilter(tr("CommonSense config files(*.cfg)"));
    fd.setDefaultSuffix(QString("cfg"));
    fd.setAcceptMode(QFileDialog::AcceptSave);
    if (fd.exec())
    {
        QStringList fns = fd.selectedFiles();
        QFile f(fns.at(0));
        f.open(QIODevice::WriteOnly);
        QDataStream ds(&f);
        ds.writeRawData((const char *)this->_eeprom.raw, sizeof(this->_eeprom.raw));
        qInfo() << "Exported config to" << fns.at(0);
        settings.setValue(DEVICECONFIG_DIR_KEY, QFileInfo(fns.at(0)).canonicalPath());
    }
}


void DeviceConfig::commit(void)
{
    QMessageBox::StandardButton result = QMessageBox::question(NULL,
            "Saving EEPROM!",
            "Do you want to write the config that is now in the device, to EEPROM?\n\nNOTICE\nIf thresholds don't fully take effect after commit\nPlease reset the device by using 'Revert' menu item!",
            QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::Yes)
        emit sendCommand(C2CMD_COMMIT, 1u);
}

void DeviceConfig::rollback(void)
{
    QMessageBox::StandardButton result = QMessageBox::question(NULL,
            "Resetting device!",
            "Device will be reset, config will be restored from EEPROM and downloaded to host. OK?",
            QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::Yes)
        emit sendCommand(C2CMD_ROLLBACK, 1u);
}

std::vector<LayerCondition> DeviceConfig::layerConditions(void)
{
    vector<LayerCondition> cnds(numLayerConditions);
    for(uint8_t i=0; i < numLayerConditions; i++)
    {
        cnds[i] = LayerCondition(_eeprom.layerConditions[i]);
    }
    return cnds;
}

void DeviceConfig::setLayerCondition(int conditionIdx, LayerCondition cnd)
{
    _eeprom.layerConditions[conditionIdx] = cnd.toBin();
}

void DeviceConfig::setLayerConditions(std::vector<LayerCondition> lcs)
{
    int numLCs = lcs.size();
    for (int i = 0; i < numLCs; i++)
        setLayerCondition(i, lcs[i]);
}
