#ifndef EVENTS_H
#define EVENTS_H
#include <QTCore>

class DeviceMessage: public QEvent {
 public:
    static const QEvent::Type ET;
    DeviceMessage(const unsigned char *buf);
    ~DeviceMessage(void);
    QByteArray* getPayload();

 private:
    QByteArray *payload;
};

#endif // EVENTS_H
