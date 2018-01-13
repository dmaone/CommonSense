#include "DeviceInterface.h"
#include <QCoreApplication>
#include <QDebug>
#include <QMessageBox>
#include <algorithm>
#include <string>

constexpr size_t kNoCtsDelay = 1000; // Ticks to wait between packets if no CTS.
constexpr size_t kNormalOperationTick = 1;
constexpr size_t kDeviceScanTick = 1000;
constexpr size_t kStatusTimerTick = 100;

DeviceInterface::DeviceInterface(QObject *parent)
    : QObject(parent), device(NULL), pollTimerId(0), statusTimerId(0),
      mode(DeviceInterfaceNormal), currentStatus(DeviceDisconnected) {
  config = new DeviceConfig();
  installEventFilter(config);

  connect(config, SIGNAL(changed()), this, SLOT(configChanged()));
  connect(config, SIGNAL(downloadBlock(c2command, uint8_t)), this,
          SLOT(sendCommand(c2command, uint8_t)));
  connect(config, SIGNAL(uploadBlock(OUT_c2packet_t)), this,
          SLOT(sendCommand(OUT_c2packet_t)));

  connect(config, SIGNAL(sendCommand(c2command, uint8_t)), this,
          SLOT(sendCommand(c2command, uint8_t)));
  statusTimerId = startTimer(kStatusTimerTick);
}

DeviceInterface::~DeviceInterface(void) { _releaseDevice(); }

void DeviceInterface::processStatusReply(QByteArray* payload) {
  if (receivedStatus_ != payload->at(1)) {
    rx = true;
  }
  receivedStatus_ = payload->at(1);
  scanEnabled = payload->at(1) & (1 << C2DEVSTATUS_SCAN_ENABLED);
  outputEnabled = payload->at(1) & (1 << C2DEVSTATUS_OUTPUT_ENABLED);
  setupMode = payload->at(1) & (1 << C2DEVSTATUS_SETUP_MODE);
  matrixMonitor = payload->at(1) & (1 << C2DEVSTATUS_MATRIX_MONITOR);
  controllerInsane = payload->at(1) & (1 << C2DEVSTATUS_INSANE);
  firmwareVersion = QString("v%1.%2")
      .arg((uint8_t)payload->at(2)).arg((uint8_t)payload->at(3));
  if (matrixMonitor) {
    setupMode = false;
  }
  emit deviceStatusNotification(StatusUpdated);
  if (!printableStatus) {
    return;
  }
  printableStatus = false;
  qInfo().nospace().noquote() << "CommonSense " << firmwareVersion
                    << ", die temp " << (payload->at(4) == 1 ? '+' : '-')
                    << (uint8_t)payload->at(5) << "C";
  qInfo().nospace() << "Scan: " << scanEnabled
      << ", Output: " << outputEnabled
      << ", Monitor: " << matrixMonitor
      << ", setup mode: " << setupMode
      << ", insane? " << controllerInsane;
}

/**
 * This is the handler of last resort for messages from device.
 * Other modules are supposed to install the event filter and process messages
 * of interest. Default behavior is to log them as strings.
 */
bool DeviceInterface::event(QEvent *e) {
  if (e->type() == DeviceMessage::ET) {
    QByteArray *payload = static_cast<DeviceMessage *>(e)->getPayload();
    switch (payload->at(0)) {
    case C2RESPONSE_STATUS:
      processStatusReply(payload);
      return true;
    case C2RESPONSE_SCANCODE:
      if (!config->bValid) {
        return true;
      }
      uint8_t scancodeReleased, scancode, row, col;
      scancodeReleased = 0x80;
      scancode = payload->at(1);
      col = (scancode & ~scancodeReleased) % config->numCols;
      row = ((scancode & ~scancodeReleased) - col) / config->numCols;
      emit scancodeReceived(
          row, col, (scancode & scancodeReleased) ? KeyReleased : KeyPressed);
      qInfo().noquote() << QString((scancode & scancodeReleased) ? "+" : " -")
                        << row + 1 << col + 1;
      return true;
    default:
      qInfo() << payload->constData();
      return true;
    }
  }
  return QObject::event(e);
}

void DeviceInterface::deviceMessageReceiver(void) {
  qInfo() << "Message received";
}

void DeviceInterface::sendCommand(c2command cmd, uint8_t *msg) {
  auto outbox = OUT_c2packet_t();
  outbox.command = cmd;
  memcpy(outbox.payload, msg, 63);
  commandQueue_.enqueue(outbox);
}

void DeviceInterface::sendCommand(c2command cmd, uint8_t msg) {
  auto outbox = OUT_c2packet_t();
  outbox.command = cmd;
  outbox.payload[0] = msg;
  commandQueue_.enqueue(outbox);
}

void DeviceInterface::sendCommand(OUT_c2packet_t cmd) {
  commandQueue_.enqueue(cmd);
}

void DeviceInterface::sendCommand(Bootloader_packet_t *packet) {
  auto outbox = OUT_c2packet_t();
  uint8_t wire_length =
      packet->length + 7; // marker+cmd+len16+checksum16+marker
  memcpy(outbox.raw, packet->raw, wire_length);
  commandQueue_.enqueue(outbox);

}

void DeviceInterface::configChanged(void) {
  qInfo() << "Configuration changed.";
  if (currentStatus == DeviceConnected) {
    emit deviceStatusNotification(DeviceConfigChanged);
  }
}

void DeviceInterface::bootloaderMode(bool bEnabled) {
  if (bEnabled) {
    qInfo() << "Entering firmware update mode.";
    mode = DeviceInterfaceBootloader;
    if (currentStatus == DeviceConnected) {
      emit sendCommand(C2CMD_ENTER_BOOTLOADER, 1);
    }
  } else {
    qInfo() << "Resuming normal operations.";
    mode = DeviceInterfaceNormal;
  }
  _resetTimer(kDeviceScanTick);
  _releaseDevice();
}

void DeviceInterface::flipStatusBit(deviceStatus bit) {
  auto newStatus = receivedStatus_;
  auto old_value = newStatus & (1 << bit);
  newStatus &= ~(1 << bit);
  if (!old_value) {
    newStatus += (1 << bit);
  }
  sendCommand(C2CMD_EWO, newStatus);
}

void DeviceInterface::_resetTimer(int interval) {
  if (pollTimerId)
    killTimer(pollTimerId);
  pollTimerId = startTimer(interval);
}

void DeviceInterface::timerEvent(QTimerEvent * timer) {
  if (timer->timerId() == statusTimerId) {
    if (currentStatus == DeviceConnected) {
        emit sendCommand(C2CMD_GET_STATUS, 1);
    }
    return;
  } else if (timer->timerId() != pollTimerId) {
    return;
  }
  if (!device)
    return _initDevice();
  _sendPacket();
  _receivePacket();
  if (releaseDevice_.exchange(false)) {
    if (device)
      qInfo("Releasing device.");
    _updateDeviceStatus(DeviceDisconnected);
    hid_close(device);
    device = NULL;
    if (hid_exit())
      qWarning("warning: error during hid_exit");
  }
}

void DeviceInterface::_sendPacket() {
  if (!device && !commandQueue_.empty()) {
    commandQueue_.clear();
    qInfo() << "Device went away on send";
  }
  if (commandQueue_.empty()) {
    return;
  }
  if (!cts_.exchange(false) && noCtsDelay_) {
    --noCtsDelay_;
    return;
  }
  noCtsDelay_ = kNoCtsDelay; // reset timer
  unsigned char outbox[65];
  auto cmd = commandQueue_.dequeue();
  outbox[0] = 0x00; // ReportID is not used.
  if (cmd.command != C2CMD_GET_STATUS) {
    tx = true;
    emit deviceStatusNotification(StatusUpdated);
  }
  memcpy(outbox+1, cmd.raw, sizeof(cmd));
  if (hid_write(device, outbox, sizeof outbox) == -1) {
    qWarning() << "Error sending to the device, will reconnect..";
    _releaseDevice();
  }
}

void DeviceInterface::_receivePacket(void) {
  memset(bytesFromDevice, 0x00, sizeof(bytesFromDevice));
  int bytesRead = hid_read(device, bytesFromDevice, sizeof(bytesFromDevice));
  if (bytesRead < 0) {
    qInfo() << "Device went away. Reconnecting..";
    _releaseDevice();
    return;
  }
  if (bytesRead == 0)
    return;
  cts_ = true;
  if (bytesFromDevice[0] != C2RESPONSE_STATUS) {
    rx = true;
  }
  QCoreApplication::postEvent(this, new DeviceMessage(bytesFromDevice));
}

void DeviceInterface::_initDevice(void) {
  device = acquireDevice();
  if (!device) {
    qInfo() << ".";
    _resetTimer(kDeviceScanTick);
    return;
  }
  cts_ = true;
  hid_set_nonblocking(device, 1);
  _updateDeviceStatus(mode == DeviceInterfaceNormal ? DeviceConnected
                                                    : BootloaderConnected);
  _resetTimer(kNormalOperationTick);
  return;
}

void DeviceInterface::_updateDeviceStatus(DeviceStatus newStatus) {
  if (newStatus != currentStatus) {
    currentStatus = newStatus;
    emit deviceStatusNotification(newStatus);
  }
}

hid_device *DeviceInterface::acquireDevice(void) {
  hid_device *retval = NULL;
  if (hid_init()) {
    qInfo() << "Cannot initialize hidapi!";
    return NULL;
  }
  hid_device_info *root = hid_enumerate(0, 0);
  if (!root) {
    qInfo() << "No HID devices on this system?";
    return NULL;
  }
  hid_device_info *d = root;
  while (d) {
    bool deviceSelected = false;
    switch (mode) {
    case DeviceInterfaceNormal:
//        qInfo() << d->path << d->vendor_id << d->product_id;
// Usage and usage page are win and mac only :(
#ifdef __linux__
      if (d->vendor_id == 0x4114)
#else
      if (d->usage_page == 0x6213 && d->usage == 0x88)
#endif
#ifdef __linux__
        if (d->interface_number == 1)
#endif
          deviceSelected = true;
      break;
    case DeviceInterfaceBootloader:
      if (d->vendor_id == 0x04b4 && d->product_id == 0xb71d)
        deviceSelected = true;
      break;
    default:
      break;
    }
    if (deviceSelected) {
      qInfo() << "Found a node!";
      qInfo() << "Trying to use" << d->path;
      retval = hid_open_path(d->path);
      if (retval)
        break;
      qInfo() << "Cannot open device. Linux permissions problem?";
    }
    d = d->next;
  }
  hid_free_enumeration(root);
  return retval;
}

void DeviceInterface::_releaseDevice(void) {
  releaseDevice_ = true;
}

void DeviceInterface::start(void) {
  qInfo() << "Acquiring device..";
  _resetTimer(kNormalOperationTick);
}
