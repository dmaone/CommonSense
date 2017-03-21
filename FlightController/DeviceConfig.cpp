#include <QFileDialog>
#include <Qfile>

#include "DeviceConfig.h"

DeviceConfig::DeviceConfig(QObject *parent) : QObject(parent),
    bValid(false), numRows(0), numCols(0), numLayers(1), transferDirection(TransferIdle)
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
            // TODO: figure out magical "31 byte offset". copy back for verification? It's 1 byte short for that.
            memcpy(msg.payload+31, this->_eeprom.raw+(CONFIG_TRANSFER_BLOCK_SIZE * currentBlock), CONFIG_TRANSFER_BLOCK_SIZE);
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
        this->_unpack();
        return;
    }
    qInfo(".");
    memcpy(this->_eeprom.raw+(CONFIG_TRANSFER_BLOCK_SIZE * (uint8_t)payload->at(1)), payload->data() + 32, CONFIG_TRANSFER_BLOCK_SIZE);
    emit(downloadBlock(C2CMD_DOWNLOAD_CONFIG, currentBlock));
}

void DeviceConfig::_unpack(void)
{
    this->numRows   = this->_eeprom.matrixRows;
    this->numCols   = this->_eeprom.matrixCols;
    this->numLayers = this->_eeprom.layerCount;
    memset(this->noiseFloor, 0xfe, sizeof(this->noiseFloor));
    memset(this->noiseCeiling, 0xfe, sizeof(this->noiseCeiling));
    memset(this->layouts, 0x00, sizeof(this->layouts));
    uint8_t table_size = this->numRows * this->numCols;
    for (uint8_t i = 0; i < this->numRows; i++)
    {
        for (uint8_t j = 0; j < this->numCols; j++)
        {
            uint16_t offset = i*this->numCols + j;
            this->noiseFloor[i][j] = this->_eeprom.stash[offset];
            this->noiseCeiling[i][j] = this->_eeprom.stash[table_size + offset];
            this->skipSensing[i][j] = (this->noiseFloor[i][j] > this->noiseCeiling[i][j]);
            for (uint8_t k = 0; k < this->numLayers; k++)
            {
                this->layouts[k][i][j] = this->_eeprom.stash[table_size*(k+2) + offset];
            }
        }
    }
    this->bValid = true;
    emit(changed());
    return;
}

void DeviceConfig::_assemble(void)
{
    memset(this->_eeprom.stash, 0, sizeof(this->_eeprom.stash));
    uint8_t table_size = this->numRows * this->numCols;
    for (uint8_t i = 0; i < this->numRows; i++)
    {
        for (uint8_t j = 0; j < this->numCols; j++)
        {
            uint16_t offset = i*this->numCols + j;
            if (this->skipSensing[i][j])
            {
                this->noiseFloor[i][j]   = 255;
                this->noiseCeiling[i][j] = 0;
            }
            this->_eeprom.stash[offset] = this->noiseFloor[i][j];
            this->_eeprom.stash[table_size + offset] = this->noiseCeiling[i][j];
            for (uint8_t k = 0; k < this->numLayers; k++)
            {
                this->_eeprom.stash[table_size*(k+2) + offset] = this->layouts[k][i][j];
            }
        }
    }
}


void DeviceConfig::fromFile()
{
    QFileDialog fd(Q_NULLPTR, "Choose one file to import from");
    fd.setNameFilter(tr("CommonSense config files(*.cfg)"));
    fd.setDefaultSuffix(QString("cfg"));
    fd.setFileMode(QFileDialog::ExistingFile);
    if (fd.exec())
    {
        QStringList fns = fd.selectedFiles();
        QFile f(fns.at(0));
        f.open(QIODevice::ReadOnly);
        QDataStream ds(&f);
        // TODO DeviceInterface &di = Singleton<DeviceInterface>::instance();
        // TODO ds.readRawData((char *)di.getConfigPtr()->raw, EEPROM_BYTESIZE);
        qInfo() << QString("Imported config from %1").arg(fns.at(0));
        //revertConfig();
    }
}

void DeviceConfig::toFile()
{
    this->_assemble();
    QFileDialog fd(Q_NULLPTR, "Choose one file to export to");
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
        qInfo() << QString("Exported config to %1").arg(fns.at(0));
    }
}
