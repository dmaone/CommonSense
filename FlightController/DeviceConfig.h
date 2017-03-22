#ifndef DEVICECONFIG_H
#define DEVICECONFIG_H

#include <QObject>
#include "Events.h"
#include "../c2/nvram.h"

class DeviceConfig : public QObject
{
    Q_OBJECT

public:
    explicit DeviceConfig(QObject *parent = 0);
    bool bValid;
    enum TransferDirection {TransferIdle, TransferUpload, TransferDownload};
    uint8_t numRows;
    uint8_t numCols;
    uint8_t numLayers;
    bool    bNormallyLow;
    uint8_t deadBandLo[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_ROWS];
    uint8_t deadBandHi[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_ROWS];
    bool    skipSensing[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_ROWS];
    uint8_t layouts[MAX_LAYERS][ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_ROWS];

signals:
    void changed(void);
    void uploadBlock(OUT_c2packet_t);
    void downloadBlock(c2command, uint8_t);

public slots:
    void fromDevice(void);
    void toDevice(void);
    void fromFile(void);
    void toFile(void);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    psoc_eeprom_t _eeprom;
    enum TransferDirection transferDirection;
    uint8_t currentBlock;
    void _uploadConfigBlock(void);
    void _receiveConfigBlock(QByteArray *);
    void _unpack(void);
    void _assemble(void);

};

#endif // DEVICECONFIG_H
