#include <QCoreApplication>
#include <string>
#include <algorithm>
#include "DeviceInterface.h"
#include "LogViewer.h"

DeviceInterface::DeviceInterface(QObject *parent): QObject(parent),
    logger(NULL), device(NULL), pollTimerId(0)
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

bool DeviceInterface::event(QEvent* e)
{
    if (e->type() == DeviceMessage::ET) {
        QByteArray *payload = static_cast<DeviceMessage *>(e)->getPayload();
        if (payload->at(0) == C2RESPONSE_STATUS)
        {
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
        }
        logger->logMessage(payload->constData());
        return true;
    }
    return QObject::event(e);
}

void DeviceInterface::deviceMessageReceiver(void)
{
    whine("Message received");
}

void DeviceInterface::sendCommand(uint8_t cmd, QByteArray msg)
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


void DeviceInterface::resetTimer(int interval)
{
    if (pollTimerId)
        killTimer(pollTimerId);
    pollTimerId = startTimer(interval);
}

void DeviceInterface::timerEvent(QTimerEvent *)
{
    if (!device) {
        start();
        if (!device)
            return;
    }
    memset(bytesFromDevice, 0x00, sizeof(bytesFromDevice));
    int bytesRead = hid_read(device, bytesFromDevice, sizeof(bytesFromDevice));
    if (bytesRead > 0)
    {
        QCoreApplication::postEvent(this, new DeviceMessage(bytesFromDevice));
    }
    else if (bytesRead < 0)
    {
        emit(deviceStatusNotification(DeviceDisconnected));
        whine("Device went away. Reconnecting..");
        start();
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

bool DeviceInterface::start(void)
{
    device = acquireDevice();
    if (device)
    {
        hid_set_nonblocking(device, 1);
        sendCommand(C2CMD_GET_STATUS, 0);
        emit(deviceStatusNotification(DeviceConnected));
        resetTimer(0);
        return true;
    }
    else
    {
        this->logger->continueMessage(".");
        resetTimer(1000);
        return false;
    }
}

void DeviceInterface::getMatrixSizeParameters(std::vector<uint8_t>& rows, std::vector<uint8_t>& cols)
{
    for (uint8_t i=0; i<ABSOLUTE_MAX_ROWS; i++)
    {
        rows.push_back(config.row_params[i].raw & 0x80 ? config.row_params[i].raw & 0x1F : 0);
    }
    for (uint8_t i=0; i<ABSOLUTE_MAX_COLS; i++)
    {
        cols.push_back(config.col_params[i].raw & 0x80 ? config.col_params[i].raw & 0x1F : 0);
    }
}

void DeviceInterface::setMatrixSizeParameters(std::vector<uint8_t> rows, std::vector<uint8_t> cols)
{
    config.matrixRows = rows.back();
    rows.pop_back();
    config.matrixCols = cols.back();
    cols.pop_back();
    for (uint8_t i=0; i<rows.size(); i++)
    {
        config.row_params[i].raw = rows[i] && i < config.matrixRows ? (rows[i] & 0x1F) | 0x80 : 0;

    }
    for (uint8_t i=0; i<cols.size(); i++)
    {
        config.col_params[i].raw = cols[i] && i < config.matrixCols ? (cols[i] & 0x1F) | 0x80 : 0;

    }
}
