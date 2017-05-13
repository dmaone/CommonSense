#ifndef DEVICECONFIG_H
#define DEVICECONFIG_H

#include <QObject>
#include "Events.h"
#include "../c2/nvram.h"
#include "LayerCondition.h"

class DeviceConfig : public QObject
{
    Q_OBJECT
    Q_ENUMS(TransferDirection)

public:
    explicit DeviceConfig(QObject *parent = 0);
    bool bValid;
    enum TransferDirection {TransferIdle, TransferUpload, TransferDownload};
    uint8_t numRows;
    uint8_t numCols;
    uint8_t numLayers;
    uint8_t numLayerConditions;
    uint8_t numDelays;
    bool    bNormallyLow;
    uint8_t guardHi;
    uint8_t guardLo;
    uint8_t deadBandLo[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
    uint8_t deadBandHi[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
    bool    skipSensing[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
    uint8_t layouts[ABSOLUTE_MAX_LAYERS][ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
    std::vector<LayerCondition> layerConditions(void);
    void setLayerCondition(int conditionIdx, LayerCondition cnd);
    void setLayerConditions(std::vector<LayerCondition> lcs);
    std::vector<uint16_t> delays(void);
    void setDelay(int delayIdx, uint16_t delay_ms);
    std::vector<uint8_t> expHeaderParams(void);
    void setExpHeaderParams(uint8_t mode, uint8_t param1, uint8_t param2);

signals:
    void changed(void);
    void uploadBlock(OUT_c2packet_t);
    void downloadBlock(c2command, uint8_t);
    void sendCommand(c2command, uint8_t);

public slots:
    void fromDevice(void);
    void toDevice(void);
    void fromFile(void);
    void toFile(void);
    void commit(void);
    void rollback(void);

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
