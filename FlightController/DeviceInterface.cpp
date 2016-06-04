#include "DeviceInterface.h"
#include "LogViewer.h"
#include <QCoreApplication>
#include <string>
#include <algorithm>


DeviceInterface::DeviceInterface(QObject *parent): QObject(parent),
    logger(NULL), device(NULL), pollTimerId(0)
{
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
    whine("Initializing device interface..");
}

void DeviceInterface::whine(QString msg)
{
    this->logger->logMessage(msg);
}

bool DeviceInterface::event(QEvent* e)
{
    if (e->type() == DeviceMessage::ET) {
        logger->logMessage(static_cast<DeviceMessage *>(e)->getPayload()->constData());
        return true;
    }
    return QObject::event(e);
}

void DeviceInterface::deviceMessageReceiver(void)
{
    whine("Message received");
}

void DeviceInterface::notifyDevice(QByteArray msg)
{
    memset(outbox, 0, sizeof(outbox));
    memcpy(outbox+1, msg.data(), std::min(msg.length(), 64));
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
        connect();
        return;
    }
    memset(bytesFromDevice, 0x00, sizeof(bytesFromDevice));
    int bytesRead = hid_read(device, bytesFromDevice, sizeof(bytesFromDevice));
    if (bytesRead > 0)
    {
//        whine(QString((char*)bytesFromDevice));
        QCoreApplication::postEvent(this, new DeviceMessage(bytesFromDevice));
    }
}

void DeviceInterface::connect(void){
    if (hid_init())
    {
        whine("Cannot initialize hidapi!");
        return;
    }
    whine("Scanning..");
    hid_device_info *root = hid_enumerate(0, 0);
    if (!root) {
        whine("No HID devices on this system?");
        resetTimer(1000);
        return;
    }
    hid_device_info *d = root;
    while (d){
          if (d->usage_page == 0xff31 && d->usage == 0x74){
            whine("Found a node!");
            device = hid_open_path(d->path);
            break;
        }
        d = d->next;
    }
    hid_free_enumeration(root);
    if (device)
    {
        hid_set_nonblocking(device, 1);
        resetTimer(0);
    }
    else
    {
        whine("Cannot open device - rescanning..");
        resetTimer(1000);
    }
}
