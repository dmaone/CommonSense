#include "FirmwareLoader.h"

#include <QFileDialog>
#include <QMessageBox>

#include "DeviceInterface.h"
#include "Events.h"
#include "settings.h"

FirmwareLoader::FirmwareLoader(DeviceInterface& di) {
  connect(this, SIGNAL(switchMode(bool)),
      &di, SLOT(bootloaderMode(bool)));
  connect(this, SIGNAL(sendPacket(Bootloader_packet_t)),
      &di, SLOT(sendCommand(Bootloader_packet_t)));

  di.installEventFilter(this);
}

bool FirmwareLoader::_checkCompatibility(const Bootloader_packet_t& packet) {
  uint32_t siliconId;
  memcpy(&siliconId, &packet.payload[0], 4);
  if (siliconId != firmware_->siliconId ||
      packet.payload[4] != firmware_->siliconRevision) {
    qInfo() << "Silicon: " << siliconId << " revision " << packet.payload[4];
    qInfo() << "Firmware: " << firmware_->siliconId
            << " revision " << firmware_->siliconRevision;
    qCritical()
        << "Firmware from incompatible hardware! Please use correct firmware!";
    return true;
  }
  qInfo().nospace() << "Remote bootloader version "
                    << (uint8_t)packet.payload[5] << "."
                    << (uint8_t)packet.payload[6] << "."
                    << (uint8_t)packet.payload[7];
  // MAYBE FLASH SIZE CHECK
  // Checking flash size gets ERR_DATA, so some other day. We check for
  // siliconId already, oughtta be enough. Let's just upload the row instead.
  // QByteArray pkt = QByteArray(0x00);
  //_sendPacket(BCMD_GetFlashSize, pkt);
  lastRow = firmware_->data.back().get();
  _upload_row();
  return true;
}

/* MAYBE FLASH SIZE CHECK

bool FirmwareLoader::_checkFlashSize(const Bootloader_packet_t& packet)
{
    qInfo() << "Checking flash size..";
    uint16_t boundaries[2];
    memcpy(boundaries, &packet.payload[0], 4);

    for(auto & el : firmware->data)
    {
        if (el->array == 0)
        {
            if (el->row < boundaries[0] || el->row > boundaries[1])
            {
                qInfo() << "Flash row " << el->row << "not present in device!";
                return true;
            }
        }
    }
    lastRow = firmware->data.back();
    _upload_row();
    return true;
}
*/

bool FirmwareLoader::_upload_row() {
  if (lastRow == nullptr) {
    _sendCommand(BCMD_ExitBootloader);
    qInfo() << "Firmware uploaded!";
    bootloaderMode = false;
    firmware_.reset();
    emit switchMode(bootloaderMode);
  } else if (lastRow->data.size() <= BOOTLOADER_MAX_PACKET_LENGTH) {
    qDebug() << "Programming array" << lastRow->array << "row" << lastRow->row;
    qInfo() << ".";
    lastRow->data.push_front((uint8_t)(lastRow->row >> 8));
    lastRow->data.push_front((uint8_t)(lastRow->row & 0xff));
    lastRow->data.push_front(lastRow->array);
    _sendPacket(BCMD_ProgramRow, lastRow->data);
    firmware_->data.pop_back();
    lastRow = (firmware_->data.empty()) ? nullptr: firmware_->data.back().get();
  } else {
    QByteArray slice = lastRow->data.left(BOOTLOADER_MAX_PACKET_LENGTH);
    _sendPacket(BCMD_SendData, slice);
    lastRow->data.remove(0, BOOTLOADER_MAX_PACKET_LENGTH);
  }
  return true;
}

bool FirmwareLoader::eventFilter(QObject* /* obj */, QEvent* event) {
  if (!bootloaderMode || event->type() != DeviceMessage::ET) {
    // Not our message
    return false;
  }
  auto pl = static_cast<DeviceMessage *>(event)->payload;
  Bootloader_packet_t packet{};
  memcpy(packet.raw, pl.constData(), pl.length());
  qDebug("Rcv: "
      "%02x%02x %02x%02x %02x%02x %02x%02x %02x%02x %02x%02x %02x%02x %02x%02x",
      packet.raw[1], packet.raw[2], packet.raw[3], packet.raw[4],
      packet.raw[5], packet.raw[6], packet.raw[7], packet.raw[8],
      packet.raw[9], packet.raw[10], packet.raw[11], packet.raw[12],
      packet.raw[13], packet.raw[14], packet.raw[15], packet.raw[16]);

  if (!_responseValid(packet)) {
    qCritical() << "Invalid packet received from bootloader!";
    return true;
  }
  if (packet.command != BR_CYRET_SUCCESS) {
    qCritical() << "Error received:" << (BootloaderVerb)packet.command;
    return true;
  }
  switch (lastCommand) {
  case BCMD_EnterBootloader:
    return _checkCompatibility(packet);
    break;
  // MAYBE FLASH SIZE CHECK
  //        case BCMD_GetFlashSize:
  //            return _checkFlashSize(packet);
  //            break;
  case BCMD_SendData:
  case BCMD_ProgramRow:
    return _upload_row();
  case BCMD_ExitBootloader:
    // Not acknowledged! Device reloads on receipt of ExitBootloader!
    break;
  default:
    qInfo() << "Received response" << (BootloaderVerb)packet.command
            << "for command" << lastCommand;
  }

  return true;
}

bool FirmwareLoader::selectFile() {
  QSettings settings;
  QFileDialog fd(nullptr, "Choose firmware file");
  fd.setDirectory(settings.value(DEVICECONFIG_DIR_KEY).toString());
  fd.setNameFilter(tr("Bootloader files(*.cyacd)"));
  fd.setDefaultSuffix(QString("cyacd"));
  fd.setFileMode(QFileDialog::ExistingFile);
  if (fd.exec()) {
    QStringList fns = fd.selectedFiles();
    QFile f(fns.at(0));
    f.open(QIODevice::ReadOnly);
    f.close();
    qInfo() << "Firmware file set to" << fns.at(0);
    settings.setValue(FIRMWARE_FILE_KEY, fns.at(0));
    settings.setValue(DEVICECONFIG_DIR_KEY,
                      QFileInfo(fns.at(0)).canonicalPath());
    return true;
  }
  return false;
}

void FirmwareLoader::start() {
  QSettings settings;
  if (!_loadFirmwareFile()) {
    qInfo("Invalid firmware file - check file integrity!");
  }
  if (bootloaderMode) {
    qInfo("Already in firmware update mode!");
    // Turns out sometimes device misses the mode switch.
    // So returning here just forces you to restart FC for no good reason.
    //return;
  }
  QMessageBox::StandardButton result = QMessageBox::question(
      nullptr, "Are you sure?",
      QString("About to flash %1\n\nYou will lose communication with the "
              "device until reset or firmware update!")
          .arg(settings.value(FIRMWARE_FILE_KEY).toString()),
      QMessageBox::Yes | QMessageBox::No);
  if (result == QMessageBox::Yes)
    bootloaderMode = true;
  emit switchMode(bootloaderMode);
}

void FirmwareLoader::load() {
  if (!firmware_) {
    if (!_loadFirmwareFile()) {
      qInfo() << "Invalid firmware file! cannot proceed!";
      return;
    }
    bootloaderMode = true;
  }
  qInfo() << "Trying to talk to bootloader";
  _sendCommand(BCMD_EnterBootloader);
}

bool FirmwareLoader::_loadFirmwareFile() {
  QSettings settings;
  QString fn = settings.value(FIRMWARE_FILE_KEY).toString();
  if (fn.length() < 1) {
    if (!selectFile())
      return false;
    fn = settings.value(FIRMWARE_FILE_KEY).toString();
  }
  try {
    firmware_ = std::make_unique<CyACD>(fn);
  } catch (const char *msg) {
    qCritical() << msg;
    return false;
  }
  qInfo() << "Successfully loaded firmware from " << fn;
  return true;
}

uint16_t FirmwareLoader::_packetChecksum(const Bootloader_packet_t& packet) {
  uint16_t checksum = 0;
  for (uint8_t i = 0; i < sizeof(packet); i++)
    checksum += packet.raw[i];
  return ~checksum + 1; // 2's complement
}

void FirmwareLoader::_sendCommand(BootloaderVerb command) {
  QByteArray data;
  _sendPacket(command, data);
}

void FirmwareLoader::_sendPacket(BootloaderVerb command, QByteArray &data) {
  lastCommand = command;

  Bootloader_packet_t packet{};
  packet.sop = BOOTLOADER_SOP_MARKER;
  packet.command = command;
  packet.length = data.length();
  memcpy(packet.payload, data.constData(), packet.length);

  auto trailer = reinterpret_cast<Bootloader_packet_trailer_t *>(
      &packet.payload[packet.length]);
  trailer->checksum = _packetChecksum(packet);
  trailer->eop = BOOTLOADER_EOP_MARKER;
  qDebug() << command;
  qDebug(
      "%02x%02x %02x%02x %02x%02x %02x%02x %02x%02x %02x%02x %02x%02x %02x%02x",
      packet.raw[1], packet.raw[2], packet.raw[3], packet.raw[4],
      packet.raw[5], packet.raw[6], packet.raw[7], packet.raw[8],
      packet.raw[9], packet.raw[10], packet.raw[11], packet.raw[12],
      packet.raw[13], packet.raw[14], packet.raw[15], packet.raw[16]);
  emit sendPacket(packet);
}

bool FirmwareLoader::_responseValid(const Bootloader_packet_t& packet) {
  if (packet.sop != BOOTLOADER_SOP_MARKER)
    return false;

  if (packet.length > BOOTLOADER_MAX_PACKET_LENGTH)
    return false;

  auto trailer = reinterpret_cast<const Bootloader_packet_trailer_t *>(
      &packet.payload[packet.length]);
  uint16_t checksum = trailer->checksum;

  if (trailer->eop != BOOTLOADER_EOP_MARKER)
    return false;

  // Clear rest of the packet
  // This is blatant const violation, but let's pretent it's not there.
  // Bootloader_packet_t is fixed size, and if checksum fails - things abort.
  memset((void *)trailer, 0, sizeof(packet.payload) - packet.length);
  return _packetChecksum(packet) == checksum;
}
