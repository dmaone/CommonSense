#pragma once

#include <QObject>
#include "../c2/nvram.h"

#include "LayerCondition.h"
#include "Macro.h"

struct HardwareConfig {
  uint8_t adcBits;
  uint8_t chargeDelay;
  uint16_t dischargeDelay;
  uint8_t debouncingTicks;
  uint8_t pedalDebouncingTicks;
  uint8_t expHdrMode;
  uint8_t expHdrParam1;
  uint8_t expHdrParam2;
  uint8_t hostMode;
};

struct SwitchTypeCapabilities {
  bool hasChargeSequencer{true};
  bool hasTelemetry{true};
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

  void reset(uint8_t mtu);

  std::vector<LayerCondition> loadLayers();
  void setLayerCondition(int conditionIdx, LayerCondition cnd);
  void setLayers(std::vector<LayerCondition> lcs);

  uint16_t getDelay(size_t delayIdx);
  void setDelay(size_t delayIdx, uint16_t delay_ms);

  HardwareConfig getHardwareConfig();
  void setHardwareConfig(HardwareConfig config);
  const std::vector<std::string> getHostModeNames();
  const std::vector<std::string> getExpModeNames();
  const QString getSwitchTypeName();
  size_t getMatrixSize() const;
  std::pair<uint8_t, uint8_t> toRowCol(uint8_t keyIndex) const;

  bool bValid{false};
  uint8_t numRows{0};
  uint8_t numCols{0};
  uint8_t numPedals{0};
  uint8_t switchType;
  uint8_t numLayers{ABSOLUTE_MAX_LAYERS};
  uint8_t numDelays{NUM_DELAYS};
  uint8_t thresholds[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
  uint8_t layouts[ABSOLUTE_MAX_LAYERS][ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
  uint8_t pedals[ABSOLUTE_MAX_PEDALS][ABSOLUTE_MAX_LAYERS];
  uint8_t pedalFlags[ABSOLUTE_MAX_PEDALS];

  SwitchTypeCapabilities capabilities{};
  std::vector<Macro> macros{};


 signals:
  void loaded();
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
  void sendConfigBlock_();
  void receiveConfigBlock_(QByteArray *);
  uint16_t getLayoutOffset_(
      uint8_t eepromVersion, uint8_t row, uint8_t col, uint8_t layer);
  void unpack_();
  void assemble_();
  void setSwitchCapabilities_();

  DeviceInterface* interface_;
  psoc_eeprom_t eeprom_;
  TransferDirection transferDirection_{TransferIdle};
  uint8_t currentBlock_{0};
  uint8_t blockSize_{0};

  const std::vector<std::string> expModeNames_{
    "Disabled",
    "Solenoid+Num+Caps",
    "Lock LEDs",
  };

  const std::vector<std::string> hostModeNames_{
      "Windows",
      "Mac",
  };

  const std::vector<std::string> switchTypeNames_{
    "CapInverted",
    "Capacitive",
    "ADB",
    "Sun",
    "Inductive",
    "Microswitch",
    "Ohmic(contact)",
    "PS/2",
    "UNKNOWN"
  };
};

