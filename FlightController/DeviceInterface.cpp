#include "DeviceInterface.h"
#include <QCoreApplication>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <algorithm>
#include <string>

constexpr size_t kNoCtsDelay = 1000; // Ticks to wait between packets if no CTS.
constexpr size_t kNormalOperationTick = 0;
constexpr size_t kDeviceScanTick = 1000;
constexpr size_t kStatusTimerTick = 200;

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

DeviceInterface::~DeviceInterface(void) { releaseDevice(); }

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
      uint8_t flags, scancode, row, col;
      uint8_t flagReleased;
      flagReleased = 0x80;
      flags = payload->at(1);
      scancode = payload->at(2);
      col = scancode % config->numCols;
      row = (scancode - col) / config->numCols;
      emit scancodeReceived(
          row, col, (flags & flagReleased) ? KeyReleased : KeyPressed);
      qInfo().noquote() << QString((flags & flagReleased) ? " r" : "p")
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

// NEVER use this function - it's only for sending packet in close event
// because timers don't work in close event!
void DeviceInterface::sendCommandNow(c2command cmd, uint8_t msg) {
  auto outbox = OUT_c2packet_t();
  outbox.command = cmd;
  outbox.payload[0] = msg;
  commandQueue_.enqueue(outbox);
  cts_.store(true); // Force it
  _sendPacket();
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
  releaseDevice();
}

void DeviceInterface::flipStatusBit(deviceStatus bit) {
  auto newStatus = receivedStatus_;
  auto oldValue = newStatus & (1 << bit);
  newStatus &= ~(1 << bit);
  if (!oldValue) {
    newStatus += (1 << bit);
  }
  sendCommand(C2CMD_EWO, newStatus);
}

void DeviceInterface::setStatusBit(deviceStatus bit, bool newValue) {
  auto newStatus = receivedStatus_;
  auto oldValue = newStatus & (1 << bit);
  if (oldValue != newValue) {
    flipStatusBit(bit);
  }
}

bool DeviceInterface::getStatusBit(deviceStatus bit) {
  return receivedStatus_ & (1 << bit);
}

void DeviceInterface::_resetTimer(int interval) {
  if (pollTimerId)
    killTimer(pollTimerId);
  pollTimerId = startTimer(interval);
}

void DeviceInterface::timerEvent(QTimerEvent * timer) {
  if (timer->timerId() == statusTimerId) {
    if (mode == DeviceInterfaceNormal && currentStatus == DeviceConnected) {
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
    emit deviceStatusNotification(StatusUpdated); // Blink the TX light
  }
  memcpy(outbox+1, cmd.raw, sizeof(cmd));
  if (hid_write(device, outbox, sizeof outbox) == -1) {
    qWarning() << "Error sending to the device, will reconnect..";
    releaseDevice();
  }
}

void DeviceInterface::_receivePacket(void) {
  memset(bytesFromDevice, 0x00, sizeof(bytesFromDevice));
  int bytesRead = hid_read(device, bytesFromDevice, sizeof(bytesFromDevice));
  if (bytesRead < 0) {
    qInfo() << "Device went away. Reconnecting..";
    releaseDevice();
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
  QByteArray tmp(5, (char)0);
  processStatusReply(&tmp);
  _resetTimer(kNormalOperationTick);
  return;
}

void DeviceInterface::_updateDeviceStatus(DeviceStatus newStatus) {
  if (newStatus != currentStatus) {
    currentStatus = newStatus;
    emit deviceStatusNotification(newStatus);
  }
}

std::vector<std::string> DeviceInterface::listDevices() {
  std::vector<std::string> retval {};
  hid_device_info *root = hid_enumerate(0, 0);
  if (!root) {
    qInfo() << "No HID devices on this system?";
    return retval;
  }
  hid_device_info *d = root;
  while (d) {
    switch (mode) {
    case DeviceInterfaceNormal:
//        qInfo() << d->path << d->vendor_id << d->product_id;
// Usage and usage page are win and mac only :(
#ifdef __linux__
      if (d->vendor_id == 0x4114 && d->interface_number == 1) {
#else
      if (d->usage_page == 0x6213 && d->usage == 0x88) {
#endif
        retval.push_back(d->path);
      }
      break;
    case DeviceInterfaceBootloader:
      if (d->vendor_id == 0x04b4 &&
          (d->product_id == 0xb71d || d->product_id == 0xf13b)) {
        retval.push_back(d->path);
      }
      break;
    default:
      break;
    }
    d = d->next;
  }
  hid_free_enumeration(root);
  return retval;
}

hid_device *DeviceInterface::acquireDevice(void) {
  hid_device *retval = NULL;
  if (hid_init()) {
    qInfo() << "Cannot initialize hidapi!";
    return NULL;
  }
  auto paths = listDevices();
  if (paths.size() == 1) {
    qInfo() << "Found a node!";
 //   qInfo() << "Trying to use" << paths[0];
    retval = hid_open_path(paths[0].data());
  } else if (paths.size() > 1) {
    // More than one device.
    qInfo() << "Hello, fellow DT member!";
    QStringList items;
    for (auto it : paths) {
      items << QString(it.data());
    }
    bool ok;
    auto device_path = QInputDialog::getItem(
        nullptr,
        tr("Please select a device"),
        tr("Path"),
        items,
        0,
        false,
        &ok
    );
    if (ok) {
      retval = hid_open_path(device_path.toStdString().data());
    }
  }
  if (!retval) {
    qInfo() << "Cannot open device. Linux permissions problem?";
  }
  return retval;
}

void DeviceInterface::releaseDevice(void) {
  releaseDevice_ = true;
}

void DeviceInterface::start(void) {
  qInfo() << "Acquiring device..";
  _resetTimer(kNormalOperationTick);
}
