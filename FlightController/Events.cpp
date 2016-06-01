#include "Events.h"
#include <QTCore>

const QEvent::Type DeviceMessage::ET = static_cast<QEvent::Type>(QEvent::registerEventType());

DeviceMessage::DeviceMessage(const unsigned char *buf): QEvent(DeviceMessage::ET)
{
    payload = new QByteArray((const char *)buf, 64);
}

DeviceMessage::~DeviceMessage(void)
{
    delete payload;
}

QByteArray* DeviceMessage::getPayload()
{
    return payload;
}
