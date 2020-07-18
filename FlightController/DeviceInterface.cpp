#include "DeviceInterface.h"

#include <QCoreApplication>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QThread>
#include <algorithm>
#include <string>

constexpr size_t kNoCtsDelay = 1000; // Ticks to wait between packets if no CTS.
constexpr size_t kNormalOperationTick{100}; // ms
constexpr size_t kDeviceScanTick{1000}; // ms connect retry
constexpr size_t kStatusTimerTick{200}; // ms between status updates
constexpr size_t kHaveDataTickMs{0}; // If we have data to send - loop FAST.
constexpr size_t kAntiLagTicks{500}; // Slow polling down after this many ticks
DeviceConfig* DeviceInterface::config{nullptr};

DeviceInterface::DeviceInterface(QObject *parent)
    : QObject(parent), device(NULL), pollTimerId(0), statusTimerId(0),
      mode(DeviceInterfaceNormal), currentStatus(DeviceDisconnected) {
  config = &config_;
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
  if (payload->size() < 6) {
    qInfo() << "Received status packet runt, length: " << payload->size();
    return;
  }
  receivedStatus_ = payload->at(1);
  scanEnabled = payload->at(1) & (1 << C2DEVSTATUS_SCAN_ENABLED);
  outputEnabled = payload->at(1) & (1 << C2DEVSTATUS_OUTPUT_ENABLED);
  setupMode = payload->at(1) & (1 << C2DEVSTATUS_SETUP_MODE);
  matrixMonitor = payload->at(1) & (1 << C2DEVSTATUS_MATRIX_MONITOR);
  controllerInsane = payload->at(1) & (1 << C2DEVSTATUS_INSANE);
  firmwareVersion = QString("%1.%2")
      .arg((uint8_t)payload->at(2)).arg((uint8_t)payload->at(3));
  dieTemp = QString("%1%2")
      .arg((payload->at(4) == 1 ? '+' : '-')).arg((uint8_t)payload->at(5));
  if (matrixMonitor) {
    setupMode = false;
  }
  emit deviceStatusNotification(StatusUpdated);
  if (!printableStatus) {
    return;
  }
  printableStatus = false;
  qInfo().nospace().noquote() << "CommonSense v" << firmwareVersion
                    << ", die temp: " << dieTemp << "°C";
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
    case C2RESPONSE_MATRIX_ROW:
        return true; // We are not interested in this, but it has >1 subscribers
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
      if (row == 15 && col == 15) {
        // "All keys released"
        qInfo() << "· ----------";
      } else {
        qInfo().noquote() <<
            QString((flags & flagReleased) ? "· r%1 c%2" : "# r%1 c%2")
            .arg(row + 1, 2)
            .arg(col + 1, 2);
      }
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

void DeviceInterface::_enqueueCommand(OUT_c2packet_t outbox) {
  {
    std::lock_guard<std::mutex> lock{queueLock_};
    commandQueue_.enqueue(outbox);
  }
  _resetTimer(kHaveDataTickMs);
}

void DeviceInterface::sendCommand(c2command cmd, uint8_t *msg) {
  OUT_c2packet_t outbox;
  outbox.command = cmd;
  memcpy(outbox.payload, msg, 63);
  qDebug() << "SendCmdMsgPtr queueLock";
  _enqueueCommand(outbox);
}

void DeviceInterface::sendCommand(c2command cmd, uint8_t msg) {
  OUT_c2packet_t outbox;
  outbox.command = cmd;
  outbox.payload[0] = msg;
  qDebug() << "SendCmdMsg queueLock";
  _enqueueCommand(outbox);
}

void DeviceInterface::sendCommand(OUT_c2packet_t cmd) {
  qDebug() << "SendCmd queueLock";
  _enqueueCommand(cmd);
}

void DeviceInterface::sendCommand(Bootloader_packet_t packet) {
  OUT_c2packet_t outbox;
  // Structure is marker+[payload]+len16+checksum16+marker
  memcpy(outbox.raw, packet.raw, packet.length + 7);
  qDebug() << "SendCmdPacketPtr queueLock";
  _enqueueCommand(outbox);
}

void DeviceInterface::configChanged(void) {
  qInfo() << "Configuration changed.";
  switchType = QString(config->getSwitchTypeName().data());
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
  if (pollTimerId) {
    killTimer(pollTimerId);
  }
  qDebug() << "Reset timer to " << interval;
  pollTimerId = startTimer(interval);
}

void DeviceInterface::timerEvent(QTimerEvent * timer) {
  if (timer->timerId() == statusTimerId) {
    if (mode == DeviceInterfaceNormal && currentStatus == DeviceConnected) {
      // request status, so you see actual status.
      emit sendCommand(C2CMD_GET_STATUS, 1);
    }
    return;
  } else if (timer->timerId() != pollTimerId) {
    return;
  }
  if (!device)
    return _initDevice();
  const auto receivedThings = _receivePacket();
  const auto sentThings = _sendPacket();
  if (receivedThings || sentThings || mode == DeviceInterfaceBootloader) {
    antiLagTimer_ = kAntiLagTicks;
  } else {
    // Go back to slow mode if idle and not bootloader.
    if (--antiLagTimer_ == 0) {
      qDebug("Idle. Slowing down to save CPU");
      _resetTimer(kNormalOperationTick);
      antiLagTimer_ = kAntiLagTicks;
    }
  }
  qDebug() << "Main timer deviceLock";
  std::lock_guard<std::mutex> lock{deviceLock_};
  if (releaseDevice_.exchange(false)) {
    if (device) {
      qInfo() << "Releasing device.";
    }
    _updateDeviceStatus(DeviceDisconnected);
    hid_close(device);
    device = NULL;
    if (hid_exit())
      qWarning("warning: error during hid_exit");
  }
  qDebug() << "Timer2 deviceUnlock";
}

bool DeviceInterface::_sendPacket() {
  unsigned char outbox[65];
  OUT_c2packet_t cmd;
  {
    qDebug() << "_sendPacket queueLock";
    std::lock_guard<std::mutex> lock{queueLock_};
    if (commandQueue_.empty()) {
      return false;
    }
    if (!device) {
      commandQueue_.clear();
      qWarning() << "Device went away on send";
      return true;
    }
    if (cts_.exchange(false) == false && --noCtsDelay_ > 0) {
      return true; // Do not slow down, may receive reply anytime soon!
    }
    noCtsDelay_ = kNoCtsDelay; // reset timer
    cmd = commandQueue_.dequeue();
  }
  outbox[0] = 0x00; // ReportID is not used.
  if (printableStatus || cmd.command != C2CMD_GET_STATUS) {
    tx = true;
    emit deviceStatusNotification(StatusUpdated); // Blink the TX light
    lastSend_ = QDateTime::currentMSecsSinceEpoch();
  }
  memcpy(outbox+1, cmd.raw, sizeof(cmd));
  qDebug() << "Sending cmd " << cmd.command << (uint8_t)cmd.payload[0];
  std::lock_guard<std::mutex> lock{deviceLock_};
  if (hid_write(device, outbox, sizeof outbox) == -1) {
    qWarning() << "Error sending to the device, will reconnect in 1 second";
    QThread::sleep(1);
    releaseDevice();
  }
  return true;
}

bool DeviceInterface::_receivePacket(void) {
  memset(bytesFromDevice, 0x00, sizeof(bytesFromDevice));
  int bytesRead;
  {
    qDebug() << "receivePacket deviceLock";
    std::lock_guard<std::mutex> lock{deviceLock_};
    bytesRead = hid_read(device, bytesFromDevice, sizeof(bytesFromDevice));
  }
  if (bytesRead == 0) {
    return false;
  } else if (bytesRead < 0) {
    qWarning() << "Device went away. Reconnecting";
    releaseDevice();
    return true;
  }
  qDebug() << "Got " << bytesRead << " b, cmd " << (uint8_t)bytesFromDevice[1];
  cts_.store(true);
  if (printableStatus || bytesFromDevice[0] != C2RESPONSE_STATUS) {
    rx = true;
    if (lastSend_ > 0) {
      latencyMs = QString("%1 ms ").arg(
          QDateTime::currentMSecsSinceEpoch() - lastSend_);
      lastSend_ = 0;
    }
  }
  QCoreApplication::postEvent(this, new DeviceMessage(bytesFromDevice));
  return rx;
}

void DeviceInterface::_initDevice(void) {
  qDebug() << "InitDevice deviceLock";
  std::lock_guard<std::mutex> lock{deviceLock_};
  device = acquireDevice();
  if (!device) {
    qInfo() << ".";
    _resetTimer(kDeviceScanTick);
    return;
  }
  hid_set_nonblocking(device, 1);

  cts_.store(true); // Not expecting anything from device - clear to send.
  _updateDeviceStatus(mode == DeviceInterfaceNormal ? DeviceConnected
                                                    : BootloaderConnected);
  QByteArray tmp(64, (char)0);
  processStatusReply(&tmp);
  // No need to change timer rate - loading config will switch it.
  return;
}

void DeviceInterface::_updateDeviceStatus(DeviceStatus newStatus) {
  if (newStatus != currentStatus) {
    currentStatus = newStatus;
    emit deviceStatusNotification(newStatus);
  }
}

DeviceInterface::DetectedDevices DeviceInterface::listDevices() {
  DetectedDevices retval{};
  hid_device_info *root = hid_enumerate(0, 0);
  if (!root) {
    qInfo() << "No HID devices on this system?";
    return retval;
  }
  hid_device_info *d = root;
  while (d) {
//        qInfo() << d->path << d->vendor_id << d->product_id;
// Usage and usage page are win and mac only :(
#ifdef __linux__
    if (d->vendor_id == 0x4114 && d->interface_number == 1) {
#else
    if (d->usage_page == 0x6213 && d->usage == 0x88) {
#endif
      retval.keyboards.push_back(
          std::make_pair(QString::fromWCharArray(d->serial_number), d->path));
    } else if (d->vendor_id == 0x04b4 &&
          (d->product_id == 0xb71d || d->product_id == 0xf13b)) {
      retval.bootloaders.push_back(std::make_pair(
          QString::fromWCharArray(d->serial_number), d->path));
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
  auto devices = listDevices();
  if (mode == DeviceInterfaceNormal && !devices.bootloaders.empty()) {
    QMessageBox::StandardButton result =
        QMessageBox::question(NULL, "Bootloader detected!",
                              "Detected an active bootloader. "
                              "Do you want to update that device's firmware?",
                              QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::Yes) {
      mode = DeviceInterfaceBootloader;
    }
  }

  DeviceList* what = mode == DeviceInterfaceNormal ? &devices.keyboards
                                                   : &devices.bootloaders;

  if (what->size() == 1) {
    qInfo() << "Found a node!";
    qDebug() << "Trying to use" << what->at(0).second.data();
    retval = hid_open_path(what->at(0).second.data());
  } else if (what->size() > 1) {
    // More than one device.
    qInfo() << "Hello, fellow DT member!";
    DeviceSelector selector{*what};
    selector.exec();
    auto deviceSerial = selector.getResult();
    qInfo().noquote() << "Connecting to s/n" << deviceSerial;
    for (auto it : *what) {
      if (it.first == deviceSerial) {
        retval = hid_open_path(it.second.data());
      }
    }
  }
  return retval;
}

void DeviceInterface::releaseDevice(void) {
  releaseDevice_ = true;
}

void DeviceInterface::start(void) {
  qInfo() << "Acquiring device";
  _resetTimer(kNormalOperationTick);
}
