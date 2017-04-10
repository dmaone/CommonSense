#ifndef DEVICECONFIG_H
#define DEVICECONFIG_H

#include <QObject>
#include "Events.h"
#include "../c2/nvram.h"

class LayerCondition
{
    public:
        enum FnKeyMask
        {
            fkmFn1 = (1 << 0),
            fkmFn2 = (1 << 1),
            fkmFn3 = (1 << 2),
            fkmFn4 = (1 << 3)
        };

        LayerCondition(void);
        LayerCondition(unsigned char bin);
        LayerCondition(bool fn1, bool fn2, bool fn3, bool fn4, int layer);
        bool fn1Set(void);
        bool fn2Set(void);
        bool fn3Set(void);
        bool fn4Set(void);
        int layer(void);
        unsigned char toBin(void);

    private:
        unsigned char fnKeys;
        unsigned char layer_;
};


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
    bool    bNormallyLow;
    uint8_t guardHi;
    uint8_t guardLo;
    uint8_t deadBandLo[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
    uint8_t deadBandHi[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
    bool    skipSensing[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
    uint8_t layouts[MAX_LAYERS][ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
    std::vector<LayerCondition> layerConditions(void);
    void setLayerCondition(int conditionIdx, LayerCondition cnd);
    void setLayerConditions(std::vector<LayerCondition> lcs);

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
