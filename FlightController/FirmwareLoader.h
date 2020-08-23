#pragma once

#include <QObject>
#include <QString>

#include "../c2/c2_protocol.h"
#include "CyACD.h"
#include "DeviceInterface.h"

#define BOOTLOADER_SOP_MARKER 0x01
#define BOOTLOADER_EOP_MARKER 0x17
class FirmwareLoader : public QObject {
  Q_OBJECT

 public:
  enum BootloaderVerb {
    BR_CYRET_SUCCESS = 0x00,
    BOOTLOADER_ERR_UNK01,
    BOOTLOADER_ERR_VERIFY,
    BOOTLOADER_ERR_LENGTH,

    BOOTLOADER_ERR_DATA,
    BOOTLOADER_ERR_CMD,
    BOOTLOADER_ERR_DEVICE,
    BOOTLOADER_ERR_VERSION,

    BOOTLOADER_ERR_CHECKSUM,
    BOOTLOADER_ERR_ARRAY,
    BOOTLOADER_ERR_ROW,
    BOOTLOADER_ERR_UNK0B,

    BOOTLOADER_ERR_APP,
    BOOTLOADER_ERR_ACTIVE,
    BOOTLOADER_ERR_UNK0E,
    BOOTLOADER_ERR_UNK,

    BCMD_VerifyChecksum = 0x31,
    BCMD_GetFlashSize,
    BCMD_GetAppStatus,
    BCMD_EraseRow,

    BCMD_Sync,
    BCMD_SetActiveApp,
    BCMD_SendData,
    BCMD_EnterBootloader,

    BCMD_ProgramRow,
    BCMD_VerifyRow,
    BCMD_ExitBootloader
  };
  Q_ENUM(BootloaderVerb)

  explicit FirmwareLoader(DeviceInterface& di);
  void load();

 public slots:
  void start();
  bool selectFile();

 signals:
  void switchMode(bool bEnable);
  void sendPacket(Bootloader_packet_t packet); // Yes, copy.

 protected:
  bool eventFilter(QObject *obj, QEvent *event);

 private:
  bool _loadFirmwareFile();
  void _sendCommand(BootloaderVerb command);
  void _sendPacket(BootloaderVerb command, QByteArray &data);
  bool _responseValid(const Bootloader_packet_t& data);
  uint16_t _packetChecksum(const Bootloader_packet_t& packet);
  bool _checkCompatibility(const Bootloader_packet_t& packet);
  bool _checkFlashSize(const Bootloader_packet_t& packet);
  bool _upload_row();

  bool bootloaderMode{false};
  std::unique_ptr<CyACD> firmware_{nullptr};
  BootloaderVerb lastCommand{BR_CYRET_SUCCESS};
  CyACD_row* lastRow;
};
