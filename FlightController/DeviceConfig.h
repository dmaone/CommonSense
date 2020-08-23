#pragma once

#include <QObject>
#include "../c2/nvram.h"

#include "Events.h"
#include "LayerCondition.h"
#include "Macro.h"

struct HardwareConfig {
  uint8_t adcBits;
  uint8_t chargeDelay;
  uint16_t dischargeDelay;
  uint8_t debouncingTicks;
  uint8_t expHdrMode;
  uint8_t expHdrParam1;
  uint8_t expHdrParam2;
};

struct SwitchTypeCapabilities {
  bool hasChargeSequencer{true};
  bool hasMatrixMonitor{true};
  bool hasThresholds{true};
  bool isNormallyLow{true};
};

class DeviceInterface;

class DeviceConfig : public QObject {
  Q_OBJECT

 public:
  enum TransferDirection { TransferIdle, TransferUpload, TransferDownload };
  Q_ENUM(TransferDirection)

  explicit DeviceConfig(DeviceInterface* di);

  std::vector<LayerCondition> loadLayers();
  void setLayerCondition(int conditionIdx, LayerCondition cnd);
  void setLayers(std::vector<LayerCondition> lcs);

  uint16_t getDelay(size_t delayIdx);
  void setDelay(size_t delayIdx, uint16_t delay_ms);

  HardwareConfig getHardwareConfig();
  void setHardwareConfig(HardwareConfig config);
  const std::vector<std::string> getExpModeNames();
  const std::string& getSwitchTypeName();

  bool bValid{false};
  uint8_t numRows{0};
  uint8_t numCols{0};
  uint8_t switchType;
  uint8_t numLayers{ABSOLUTE_MAX_LAYERS};
  uint8_t numDelays{NUM_DELAYS};
  uint8_t thresholds[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
  uint8_t layouts[ABSOLUTE_MAX_LAYERS][ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
  SwitchTypeCapabilities capabilities{};
  std::vector<Macro> macros{};


 signals:
  void changed();
  void uploadBlock(OUT_c2packet_t);
  void downloadBlock(c2command, uint8_t);
  void sendCommand(c2command, uint8_t);

 public slots:
  void fromDevice();
  void toDevice();
  void fromFile();
  void toFile();
  void commit();
  void rollback();

 protected:
  bool eventFilter(QObject *obj, QEvent *event);

 private:
  void _uploadConfigBlock();
  void _receiveConfigBlock(QByteArray *);
  void _unpack();
  void _assemble();
  void _setSwitchCapabilities();

  DeviceInterface* interface_;
  psoc_eeprom_t _eeprom;
  TransferDirection transferDirection_{TransferIdle};
  uint8_t currentBlock{0};
};

