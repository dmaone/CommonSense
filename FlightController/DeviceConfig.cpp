#include <QFileDialog>
#include <Qfile>

#include "DeviceConfig.h"

DeviceConfig::DeviceConfig(QObject *parent) : QObject(parent),
    transferDirection(TransferIdle)
{
    memset(config.raw, 0x00, sizeof(config));
}

bool DeviceConfig::eventFilter(QObject *obj __attribute__((unused)), QEvent *event){
    if (event->type() == DeviceMessage::ET )
    {
        QByteArray *payload = static_cast<DeviceMessage *>(event)->getPayload();
        if (payload->at(0) == C2RESPONSE_CONFIG)
        {
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
    }
    return false;
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
    transferDirection = TransferUpload;
    _assembleConfig();
    currentBlock = 0;
    qInfo() << "Uploading config..";
    _uploadConfigBlock();
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
            uint8_t payload[63];
            payload[0] = currentBlock;
            memcpy(payload+31, config.raw+(CONFIG_TRANSFER_BLOCK_SIZE * currentBlock), CONFIG_TRANSFER_BLOCK_SIZE);
            emit(uploadBlock(C2CMD_UPLOAD_CONFIG, payload));
            break;
        default:
            break;
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
            qInfo() << "Already downloading! Re-requesting current block";
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
    if (currentBlock > (EEPROM_BYTESIZE / CONFIG_TRANSFER_BLOCK_SIZE))
    {
        qInfo() << "done!";
        transferDirection = TransferIdle;
        emit(changed());
        return;
    }
    qInfo(".");
    memcpy(config.raw+(CONFIG_TRANSFER_BLOCK_SIZE * (uint8_t)payload->at(1)), payload->data() + 32, CONFIG_TRANSFER_BLOCK_SIZE);
    emit(downloadBlock(C2CMD_DOWNLOAD_CONFIG, currentBlock));
}

void DeviceConfig::_assembleConfig(void)
{

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
        // TODO DeviceInterface &di = Singleton<DeviceInterface>::instance();
        // TODO ds.writeRawData((const char *)di.getConfigPtr()->raw, EEPROM_BYTESIZE);
        qInfo() << QString("Exported config to %1").arg(fns.at(0));
    }
}
