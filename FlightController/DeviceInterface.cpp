#include <QCoreApplication>
#include <QMessageBox>
#include <string>
#include <algorithm>
#include "DeviceInterface.h"
#include "LogViewer.h"

DeviceInterface::DeviceInterface(QObject *parent): QObject(parent),
    logger(NULL), device(NULL), pollTimerId(0)
{
    config = new DeviceConfig();
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
    logger->logMessage("Initializing logging interface..");
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
            logger->logMessage(QString("CommonSense v%1.%2, die temp %3%4 C")
                  .arg((uint8_t)payload->at(2))
                  .arg((uint8_t)payload->at(3))
                  .arg((uint8_t)payload->at(4) == 1 ? '+' : '-')
                  .arg((uint8_t)payload->at(5))
            );
            logger->logMessage(QString("Quenched: %1, Matrix monitoring: %2")
                  .arg((payload->at(1) & (1 << C2DEVSTASTUS_EMERGENCY)) ? "Yes":"No")
                  .arg((payload->at(1) & (1 << C2DEVSTASTUS_MATRIXOUTPUT)) ? "Yes":"No")
            );
            return true;
        default:
            logger->logMessage(payload->constData());
            return true;
        }
    }
    return QObject::event(e);
}

void DeviceInterface::deviceMessageReceiver(void)
{
    logger->logMessage("Message received");
}

void DeviceInterface::_sendPacket()
{
    if (!device) return; // TODO we should be more vocal
    hid_write(device, outbox, sizeof(outbox));
}

void DeviceInterface::sendCommand(uint8_t cmd, uint8_t *msg)
{
    memset(outbox, 0, sizeof(outbox));
    outbox[1] = cmd;
    memcpy(outbox+2, msg, 63);
    _sendPacket();
}

void DeviceInterface::sendCommand(uint8_t cmd, QByteArray &msg)
{
    memset(outbox, 0, sizeof(outbox));
    outbox[1] = cmd;
    memcpy(outbox+2, msg.data(), std::min(msg.length(), 63));
    _sendPacket();
}

void DeviceInterface::sendCommand(uint8_t cmd, uint8_t msg)
{
    memset(outbox, 0, sizeof(outbox));
    outbox[1] = cmd;
    outbox[2] = msg;
    _sendPacket();
}

void DeviceInterface::_resetTimer(int interval)
{
    if (pollTimerId) killTimer(pollTimerId);
    pollTimerId = startTimer(interval);
}

void DeviceInterface::timerEvent(QTimerEvent *)
{
    if (!device)
        return _initDevice();

    memset(bytesFromDevice, 0x00, sizeof(bytesFromDevice));
    int bytesRead = hid_read(device, bytesFromDevice, sizeof(bytesFromDevice));
    if (bytesRead < 0)
    {
        logger->logMessage("Device went away. Reconnecting..");
        hid_close(device);
        device = NULL;
        emit(deviceStatusNotification(DeviceDisconnected));
        return;
    }
    if (bytesRead == 0)
        return;

    QCoreApplication::postEvent(this, new DeviceMessage(bytesFromDevice));
}

void DeviceInterface::_initDevice(void)
{
    device = acquireDevice();
    if (!device) {
        this->logger->continueMessage(".");
        _resetTimer(1000);
    }
    hid_set_nonblocking(device, 1);
    emit(deviceStatusNotification(DeviceConnected));
    _resetTimer(0);
    return;
}

hid_device* DeviceInterface::acquireDevice(void)
{
    hid_device *retval = NULL;
    if (hid_init())
    {
        logger->logMessage("Cannot initialize hidapi!");
        return NULL;
    }
    hid_device_info *root = hid_enumerate(0, 0);
    if (!root) {
        logger->logMessage("No HID devices on this system?");
        return NULL;
    }
    hid_device_info *d = root;
    while (d){
          if (d->usage_page == 0x6213 && d->usage == 0x88){
            logger->logMessage("Found a node!");
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
    qInfo() << "Acquiring device..";
    _resetTimer(0);
}
