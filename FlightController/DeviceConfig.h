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
    psoc_eeprom_t *getConfigPtr(void) { return &config; }
    enum TransferDirection {TransferIdle, TransferUpload, TransferDownload};
    void getMatrixSizeParameters(std::vector<uint8_t>&, std::vector<uint8_t>&);
    void setMatrixSizeParameters(std::vector<uint8_t>, std::vector<uint8_t>);

signals:
    void changed(void);
    void uploadBlock(uint8_t, uint8_t*);
    void downloadBlock(uint8_t, uint8_t);

public slots:
    void fromDevice(void);
    void toDevice(void);
    void fromFile(void);
    void toFile(void);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    psoc_eeprom_t config;
    enum TransferDirection transferDirection;
    uint8_t currentBlock;
    void _uploadConfigBlock(void);
    void _receiveConfigBlock(QByteArray *);
    void _assembleConfig(void);

};

#endif // DEVICECONFIG_H
