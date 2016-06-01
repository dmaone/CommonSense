#include "DeviceInterface.h"
#include "LogViewer.h"
#include <QCoreApplication>
#include <string>


DeviceInterface::DeviceInterface(QObject *parent): QObject(parent),
    device(NULL), logger(NULL), listener(NULL)
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

void DeviceInterface::deviceMessageReceiver(void)
{
    whine("Message received");
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
        whine(QString((char*)bytesFromDevice));
//        whine(QString("bytes read: %1").arg(bytesRead));
        QCoreApplication::postEvent(logger, new DeviceMessage(bytesFromDevice));
    }
}

void DeviceInterface::installListener(void)
{
    if (this->listener) {
        // Connection crashed?
        delete this->listener;
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
