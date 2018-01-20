#include <QFileDialog>
#include <QMessageBox>

#include "singleton.h"

#include "CyACD.h"
#include "DeviceInterface.h"
#include "Events.h"
#include "FirmwareLoader.h"
#include "settings.h"

FirmwareLoader::FirmwareLoader(QObject *parent)
    : QObject(parent), bootloaderMode(false), firmware(NULL),
      lastCommand(BR_CYRET_SUCCESS) {
  DeviceInterface &di = Singleton<DeviceInterface>::instance();
  di.installEventFilter(this);
}

bool FirmwareLoader::_checkCompatibility(Bootloader_packet_t *packet) {
  uint32_t siliconId;
  memcpy(&siliconId, &packet->payload[0], 4);
  if (siliconId != firmware->siliconId ||
      packet->payload[4] != firmware->siliconRevision) {
    qCritical()
        << "Firmware from incompatible hardware! Please use correct firmware!";
    return true;
  }
  qInfo().nospace() << "Remote bootloader version "
                    << (uint8_t)packet->payload[5] << "."
                    << (uint8_t)packet->payload[6] << "."
                    << (uint8_t)packet->payload[7];
  // MAYBE FLASH SIZE CHECK
  // Checking flash size gets ERR_DATA, so some other day. We check for
  // siliconId already, oughtta be enough. Let's just upload the row instead.
  // QByteArray pkt = QByteArray(0x00);
  //_sendPacket(BCMD_GetFlashSize, pkt);
  lastRow = firmware->data.back();
  _upload_row();
  return true;
}

/* MAYBE FLASH SIZE CHECK

bool FirmwareLoader::_checkFlashSize(Bootloader_packet_t *packet)
{
    qInfo() << "Checking flash size..";
    uint16_t boundaries[2];
    memcpy(boundaries, &packet->payload[0], 4);

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

bool FirmwareLoader::_upload_row(void) {
  if (lastRow == NULL) {
    _sendCommand(BCMD_ExitBootloader);
    qInfo() << "Firmware uploaded!";
    bootloaderMode = false;
    delete firmware;
    firmware = NULL;
    emit switchMode(bootloaderMode);
  } else if (lastRow->data.size() <= BOOTLOADER_MAX_PACKET_LENGTH) {
    //qInfo() << "Programming array" << lastRow->array << "row" << lastRow->row;
    qInfo() << ".";
    lastRow->data.push_front((uint8_t)(lastRow->row >> 8));
    lastRow->data.push_front((uint8_t)(lastRow->row & 0xff));
    lastRow->data.push_front(lastRow->array);
    _sendPacket(BCMD_ProgramRow, lastRow->data);
    firmware->data.pop_back();
    lastRow = (firmware->data.size() > 0) ? firmware->data.back() : NULL;
  } else {
    QByteArray slice = lastRow->data.left(BOOTLOADER_MAX_PACKET_LENGTH);
    _sendPacket(BCMD_SendData, slice);
    lastRow->data.remove(0, BOOTLOADER_MAX_PACKET_LENGTH);
  }
  return true;
}

bool FirmwareLoader::eventFilter(QObject *obj __attribute__((unused)),
                                 QEvent *event) {
  if (!(bootloaderMode && event->type() == DeviceMessage::ET))
    return false;
  QByteArray *pl = static_cast<DeviceMessage *>(event)->getPayload();
  Bootloader_packet_t *packet = (Bootloader_packet_t *)pl->constData();
  //    qInfo().noquote() << QString::number(packet->raw[0], 16) <<
  //    QString::number(packet->raw[1], 16) << QString::number(packet->raw[2],
  //    16) << QString::number(packet->raw[3], 16)
  //            << QString::number(packet->raw[4], 16) <<
  //            QString::number(packet->raw[5], 16) <<
  //            QString::number(packet->raw[6], 16) <<
  //            QString::number(packet->raw[7], 16)
  //            << QString::number(packet->raw[8], 16) <<
  //            QString::number(packet->raw[9], 16) <<
  //            QString::number(packet->raw[10], 16) <<
  //            QString::number(packet->raw[11], 16)
  //            << QString::number(packet->raw[12], 16) <<
  //            QString::number(packet->raw[13], 16) <<
  //            QString::number(packet->raw[14], 16) <<
  //            QString::number(packet->raw[15], 16);
  if (!_responseValid(packet)) {
    qCritical() << "Invalid packet received from bootloader!";
    return true;
  }
  if (packet->command != BR_CYRET_SUCCESS) {
    qCritical() << "Error received:" << (BootloaderVerb)packet->command;
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
    qInfo() << "Received response" << (BootloaderVerb)packet->command
            << "for command" << lastCommand;
  }

  return true;
}

bool FirmwareLoader::selectFile(void) {
  QSettings settings;
  QFileDialog fd(Q_NULLPTR, "Choose firmware file");
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

void FirmwareLoader::start(void) {
  QSettings settings;
  if (!_loadFirmwareFile()) {
    qInfo("Invalid firmware file - check file integrity!");
  }
  if (bootloaderMode) {
    qInfo("Already in firmware update mode!");
    return;
  }
  QMessageBox::StandardButton result = QMessageBox::question(
      NULL, "Are you sure?",
      QString("About to flash %1\n\nYou will lose communication with the "
              "device until reset or firmware update!")
          .arg(settings.value(FIRMWARE_FILE_KEY).toString()),
      QMessageBox::Yes | QMessageBox::No);
  if (result == QMessageBox::Yes)
    bootloaderMode = true;
  emit switchMode(bootloaderMode);
}

void FirmwareLoader::load(void) {
  if (!firmware) {
    qInfo() << "Invalid firmware file! cannot proceed!";
    return;
  }
  _sendCommand(BCMD_EnterBootloader);
}

bool FirmwareLoader::_loadFirmwareFile(void) {
  QSettings settings;
  QString fn = settings.value(FIRMWARE_FILE_KEY).toString();
  if (fn.length() < 1) {
    if (!selectFile())
      return false;
    fn = settings.value(FIRMWARE_FILE_KEY).toString();
  }
  if (firmware)
    delete firmware;
  try {
    firmware = new CyACD(fn);
  } catch (const char *msg) {
    qCritical() << msg;
    return false;
  }
  qInfo() << "Firmware file loaded";
  return true;
}

uint16_t FirmwareLoader::_packetChecksum(Bootloader_packet_t *packet) {
  uint16_t checksum = 0;
  for (uint8_t i = 0; i < sizeof *packet; i++)
    checksum += packet->raw[i];
  return ~checksum + 1; // 2's complement
}

void FirmwareLoader::_sendCommand(BootloaderVerb command) {
  QByteArray data;
  _sendPacket(command, data);
}

void FirmwareLoader::_sendPacket(BootloaderVerb command, QByteArray &data) {
  // qInfo() << "Sending" << command;
  lastCommand = command;

  Bootloader_packet_t *packet = new Bootloader_packet_t();
  packet->sop = BOOTLOADER_SOP_MARKER;
  packet->command = command;
  packet->length = data.length();
  memcpy(packet->payload, data.constData(), packet->length);

  Bootloader_packet_trailer_t *trailer =
      (Bootloader_packet_trailer_t *)&packet->payload[packet->length];
  trailer->checksum = _packetChecksum(packet);
  trailer->eop = BOOTLOADER_EOP_MARKER;
  //    qInfo().noquote() << QString::number(packet->raw[0], 16) <<
  //    QString::number(packet->raw[1], 16) << QString::number(packet->raw[2],
  //    16) << QString::number(packet->raw[3], 16)
  //            << QString::number(packet->raw[4], 16) <<
  //            QString::number(packet->raw[5], 16) <<
  //            QString::number(packet->raw[6], 16) <<
  //            QString::number(packet->raw[7], 16)
  //            << QString::number(packet->raw[8], 16) <<
  //            QString::number(packet->raw[9], 16) <<
  //            QString::number(packet->raw[10], 16) <<
  //            QString::number(packet->raw[11], 16)
  //            << QString::number(packet->raw[12], 16) <<
  //            QString::number(packet->raw[13], 16) <<
  //            QString::number(packet->raw[14], 16) <<
  //            QString::number(packet->raw[15], 16);
  emit sendPacket(packet);
  delete packet;
}

bool FirmwareLoader::_responseValid(Bootloader_packet_t *packet) {
  if (packet->sop != BOOTLOADER_SOP_MARKER)
    return false;

  if (packet->length > BOOTLOADER_MAX_PACKET_LENGTH)
    return false;

  Bootloader_packet_trailer_t *trailer =
      (Bootloader_packet_trailer_t *)&packet->payload[packet->length];
  uint16_t checksum = trailer->checksum;

  if (trailer->eop != BOOTLOADER_EOP_MARKER)
    return false;

  // Clear rest of the packet
  memset((void *)trailer, 0, sizeof(packet->payload) - packet->length);
  return _packetChecksum(packet) == checksum;
}
