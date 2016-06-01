#ifndef DEVICEINTERFACE_H
#define DEVICEINTERFACE_H
#include "hidapi.h"
#include "Events.h"
#include <QObject>
#include "LogViewer.h"

class DeviceInterface : public QObject
{
    Q_OBJECT

    public:
        DeviceInterface(QObject *parent = 0);
        ~DeviceInterface();
        void setLogger(LogViewer *l);
        void connect(void);
        void listen(void);

    protected:
        virtual void timerEvent(QTimerEvent *);

    private:
        hid_device* device;
        LogViewer* logger;
        QSocketNotifier* listener;
        int pollTimerId;
        unsigned char bytesFromDevice[128];
        void installListener(void);
        void whine(QString msg);
        void resetTimer(int interval);

    private slots:
        void deviceMessageReceiver(void);

};

#endif // DEVICEINTERFACE_H
