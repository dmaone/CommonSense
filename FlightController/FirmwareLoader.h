#ifndef FIRMWARLOADER_H
#define FIRMWARLOADER_H

#include <QObject>

class FirmwareLoader : public QObject
{
    Q_OBJECT
public:
    explicit FirmwareLoader(QObject *parent = 0);

signals:

public slots:
};

#endif // FIRMWARLOADER_H