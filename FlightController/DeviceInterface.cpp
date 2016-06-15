#include <QCoreApplication>
#include <string>
#include <algorithm>
#include "DeviceInterface.h"
#include "LogViewer.h"

DeviceInterface::DeviceInterface(QObject *parent): QObject(parent),
    logger(NULL), device(NULL), pollTimerId(0), transferDirection(TransferIdle)
{
    memset(config.raw, 0x00, sizeof(config));
}

DeviceInterface::~DeviceInterface(void)
{
    if (device)
        hid_close(device);
    if(hid_exit())
        qWarning("warning: error during hid_exit");
}

void DeviceInterface::setLogger(LogViewer* l)
{
    this->logger = l;
    whine("Initializing logging interface..");
}

void DeviceInterface::whine(QString msg)
{
    this->logger->logMessage(msg);
}

/**
 * This is the handler of last resort for messages from device.
 * Other modules are supposed to install the event filter and process messages of interest.
 * Default behavior is to output them to the log window as strings.
 */
bool DeviceInterface::event(QEvent* e)
{
    if (e->type() == DeviceMessage::ET) {
        QByteArray *payload = static_cast<DeviceMessage *>(e)->getPayload();
        switch (payload->at(0))
        {
        case C2RESPONSE_STATUS:
            whine(QString("CommonSense v%1.%2, die temp %3%4 C")
                  .arg((uint8_t)payload->at(2))
                  .arg((uint8_t)payload->at(3))
                  .arg((uint8_t)payload->at(4) == 1 ? '+' : '-')
                  .arg((uint8_t)payload->at(5))
            );
            whine(QString("Quenched: %1, Matrix monitoring: %2")
                  .arg((payload->at(1) & (1 << C2DEVSTASTUS_EMERGENCY)) ? "Yes":"No")
                  .arg((payload->at(1) & (1 << C2DEVSTASTUS_MATRIXOUTPUT)) ? "Yes":"No")
            );
            return true;
        case C2RESPONSE_CONFIG:
            currentBlock++;
            switch (transferDirection)
            {
            case TransferDownload:
                downloadConfigBlock(payload);
                return true;
            case TransferUpload:
                uploadConfigBlock();
                return true;
            default:
                whine(QString("Received config block %1 while supposed to be idle!").arg((uint8_t)payload->at(1)));
                return true;
            }
        default:
            logger->logMessage(payload->constData());
            return true;
        }
    }
    return QObject::event(e);
}

void DeviceInterface::deviceMessageReceiver(void)
{
    whine("Message received");
}

void DeviceInterface::sendCommand(uint8_t cmd, uint8_t *msg)
{
    memset(outbox, 0, sizeof(outbox));
    outbox[1] = cmd;
    memcpy(outbox+2, msg, 63);
    hid_write(device, outbox, sizeof(outbox));
}

void DeviceInterface::sendCommand(uint8_t cmd, QByteArray &msg)
{
    memset(outbox, 0, sizeof(outbox));
    outbox[1] = cmd;
    memcpy(outbox+2, msg.data(), std::min(msg.length(), 63));
    hid_write(device, outbox, sizeof(outbox));
}

void DeviceInterface::sendCommand(uint8_t cmd, uint8_t msg)
{
    memset(outbox, 0, sizeof(outbox));
    outbox[1] = cmd;
    outbox[2] = msg;
    hid_write(device, outbox, sizeof(outbox));
}

/**
 * @brief DeviceInterface::uploadConfig
 * Fire up the uploader.
 */
void DeviceInterface::uploadConfig(void)
{
    switch(transferDirection)
    {
        case TransferIdle:
            transferDirection = TransferUpload;
            currentBlock = 0;
            whine("Uploading config..");
            uploadConfigBlock();
            break;
    default:
        whine("Not a good day to upload config!");
    }
}

/**
 * @brief DeviceInterface::uploadConfigBlock
 * Upload one block to device.
 * We don't really care about the input packet here, hence (void)
 */
void DeviceInterface::uploadConfigBlock(void)
{
    switch(transferDirection)
    {
        case TransferUpload:
            if (currentBlock > (EEPROM_BYTESIZE / CONFIG_TRANSFER_BLOCK_SIZE))
            {
                whine ("done!");
                transferDirection = TransferIdle;
                return;
            }
            this->logger->continueMessage(".");
            uint8_t payload[63];
            payload[0] = currentBlock;
            memcpy(payload+31, config.raw+(CONFIG_TRANSFER_BLOCK_SIZE * currentBlock), CONFIG_TRANSFER_BLOCK_SIZE);
            sendCommand(C2CMD_UPLOAD_CONFIG, payload);
            break;
    default:
        whine("Not a good day to upload config block!");
    }

}

void DeviceInterface::downloadConfig()
{
    switch(transferDirection)
    {
    case TransferIdle:
        transferDirection = TransferDownload;
        currentBlock = 0;
        whine("Downloading config..");
        break;
    case TransferDownload:
        whine("Already downloading! Re-requesting current block");
        break;
    default:
        whine("Not a good day to download config!");
        return;
    }
    sendCommand(C2CMD_DOWNLOAD_CONFIG, currentBlock);
}

/**
 * @brief DeviceInterface::downloadConfigBlock
 * Receives one block from device, writes it to local config.
 * @param payload - packet payload
 */
void DeviceInterface::downloadConfigBlock(QByteArray *payload)
{
    switch(transferDirection)
    {
        case TransferDownload:
            if (currentBlock > (EEPROM_BYTESIZE / CONFIG_TRANSFER_BLOCK_SIZE))
            {
                whine ("done!");
                transferDirection = TransferIdle;
                emit(deviceStatusNotification(DeviceConfigLoaded));
                return;
            }
            this->logger->continueMessage(".");
            memcpy(config.raw+(CONFIG_TRANSFER_BLOCK_SIZE * (uint8_t)payload->at(1)), payload->data() + 32, CONFIG_TRANSFER_BLOCK_SIZE);
            sendCommand(C2CMD_DOWNLOAD_CONFIG, currentBlock);
            break;
    default:
        whine("Not a good day to download config block!");
    }
}

void DeviceInterface::resetTimer(int interval)
{
    if (pollTimerId)
        killTimer(pollTimerId);
    pollTimerId = startTimer(interval);
}

void DeviceInterface::timerEvent(QTimerEvent *)
{
    if (!device)
    {
        device = acquireDevice();
        if (!device) {
            emit(deviceStatusNotification(DeviceDisconnected));
            this->logger->continueMessage(".");
            resetTimer(1000);
            return;
        }
        hid_set_nonblocking(device, 1);
        //sendCommand(C2CMD_GET_STATUS, (uint8_t)0);
        //need to be careful not to send too fast!
        downloadConfig();
        emit(deviceStatusNotification(DeviceConnected));
        resetTimer(0);
    }
    memset(bytesFromDevice, 0x00, sizeof(bytesFromDevice));
    int bytesRead = hid_read(device, bytesFromDevice, sizeof(bytesFromDevice));
    if (bytesRead > 0)
    {
        QCoreApplication::postEvent(this, new DeviceMessage(bytesFromDevice));
    }
    else if (bytesRead < 0)
    {
        whine("Device went away. Reconnecting..");
        hid_close(device);
        device = NULL;
    }
}

hid_device* DeviceInterface::acquireDevice(void)
{
    hid_device *retval = NULL;
    if (hid_init())
    {
        whine("Cannot initialize hidapi!");
        return NULL;
    }
    hid_device_info *root = hid_enumerate(0, 0);
    if (!root) {
        whine("No HID devices on this system?");
        return NULL;
    }
    hid_device_info *d = root;
    while (d){
          if (d->usage_page == 0xff31 && d->usage == 0x74){
            whine("Found a node!");
            retval = hid_open_path(d->path);
            break;
        }
        d = d->next;
    }
    hid_free_enumeration(root);
    return retval;
}

void DeviceInterface::start(void)
{
    whine("Acquiring device..");
    resetTimer(0);
}
