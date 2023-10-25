#include "DeviceInterface.h"

#include <QCoreApplication>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QThread>
#include <algorithm>
#include <string>

#include "Events.h"
#include "ScancodeList.h"

namespace {
constexpr bool kDebugUsbTraffic{false};
constexpr bool kDumpIncomingPackets{false};

constexpr size_t kMaxPacketSendRetries{3};
constexpr size_t kNoCtsDelayMs{1000}; // Wait this long for CTS.
constexpr size_t kNormalOperationTick{100}; // ms
constexpr size_t kDeviceScanTick{1000}; // ms connect retry
constexpr size_t kStatusTimerTick{200}; // ms between status updates
constexpr size_t kHaveDataTickMs{2}; // If we have data to send - loop FAST.
constexpr size_t kAntiLagTicks{500}; // Slow polling down after this many ticks



// Constants below are non-negotiable
constexpr uint8_t kNoReport{0};
constexpr int kAbsolutelyNoRetriesLeft{-1};

void printHidError(hid_device_ * device) {
  qWarning() << QString::fromWCharArray(hid_error(device));
}

}

DeviceInterface::DeviceInterface() : QObject{nullptr}, config{this} {
  installEventFilter(&config);

  connect(&config, SIGNAL(loaded()), this, SLOT(configLoaded()));
  connect(&config, SIGNAL(downloadBlock(c2command, uint8_t)), this,
          SLOT(sendCommand(c2command, uint8_t)));
  connect(&config, SIGNAL(uploadBlock(OUT_c2packet_t)), this,
          SLOT(sendCommand(OUT_c2packet_t)));

  connect(&config, SIGNAL(sendCommand(c2command, uint8_t)), this,
          SLOT(sendCommand(c2command, uint8_t)));
  statusTimerId_ = startTimer(kStatusTimerTick);
}

DeviceInterface::~DeviceInterface() {
  if (pollTimerId_ > 0) {
    killTimer(pollTimerId_);
  }
  scheduleDeviceRelease_.store(true);
  releaseDeviceIfScheduled_(); // So we don't wait for timer
}

QString DeviceInterface::formatKey_(
    const uint8_t keyIndex, const uint8_t flags) const {
  auto [row, col] = config.toRowCol(keyIndex);
  bool keyUp = flags & 0x80;
  if (keyIndex > config.getMatrixSize()) {
    return QString("%1 p%2").arg(keyUp ? "·": "#").arg(col + 1, 2);
  }
  return QString("%1 r%2 c%3")
      .arg(keyUp ? "·": "#").arg(row + 1, 2).arg(col + 1, 2);

}

QString DeviceInterface::formatSysTime_(const uint32_t sysTime) const {
  return QString("t+%1.%2 ")
      .arg((uint32_t)sysTime/1000)
      .arg(sysTime % 1000, 3, 10, QLatin1Char('0'));
}

void DeviceInterface::decodeMessage_(const QByteArray& payload) {
  auto msg = reinterpret_cast<const coded_message_t*>(payload.constData());
  switch (msg->messageCode) {
    case MC_KEYPRESS: {
      auto p = reinterpret_cast<const mc_keypress_t*>(msg->message);
      if (!config.bValid) {
        qWarning() << "Received key " << p->key << " on invalid config!";
        return;
      }
      if (p->key == COMMONSENSE_NOKEY) {
        // "All keys released"
        qInfo() << "---------";
        return;
      }
      auto matrixSize = config.getMatrixSize();
      bool keyUp = p->flags & 0x80;
      if (p->key >= matrixSize + config.numPedals) {
        qWarning() << "Invalid key: " << p->key << formatKey_(p->key, p->flags);
        return;
      }
      qInfo().noquote() << formatKey_(p->key, p->flags);
      if (p->key >= matrixSize) {
        emit pedal(p->key - matrixSize, keyUp ? KeyReleased : KeyPressed);
      } else {
        emit keypress(p->key, keyUp ? KeyReleased : KeyPressed);
      }
      break;
    }
    case MC_KEY_RESOLVED: {
      auto p = reinterpret_cast<const mc_key_resolved_payload_t*>(msg->message);
      QString s{};
      auto out = QTextStream{&s};
      out << formatSysTime_(msg->sysTime) << formatKey_(p->key, p->flags)
          << " @L" << p->layer  << "-> " << ScancodeList::asString(p->code);
      out.flush();
      qInfo().noquote().nospace() << s;
      break;
    }
    case MC_SCHEDULE_HID: {
      auto p = reinterpret_cast<const mc_schedule_hid_payload_t*>(msg->message);
      int delta = p->event_time - msg->sysTime;
      QString s{};
      auto out = QTextStream{&s};
      out << formatSysTime_(msg->sysTime)
          << "Queued " << QString((p->flags & HID_RELEASED_MASK) ? "· ": "# ")
          << ScancodeList::asString(p->code)
          << ((p->flags & HID_REAL_KEY_MASK) ? "(real)": "")
          << " @" << formatSysTime_(p->event_time) << "(" << Qt::forcesign
          << delta << Qt::noforcesign << " ms) pos " << p->position
          << ". Buffer: " << p->data_begin << " to " << p->data_end;
        out.flush();
        qInfo().noquote().nospace() << s;
      break;
    }
    case MC_PROCESS_HID: {
      auto p = reinterpret_cast<const mc_process_hid_payload_t*>(msg->message);
      int delta = p->event_time - msg->sysTime;
      QString s{};
      auto out = QTextStream{&s};
      out << formatSysTime_(msg->sysTime)
          << "Applying " << QString((p->flags & HID_RELEASED_MASK) ? "· ": "# ")
          << ScancodeList::asString(p->code)
          << ((p->flags & HID_REAL_KEY_MASK) ? "(real)": "")
          << " @" << formatSysTime_(p->event_time) << "(" << Qt::forcesign
          << delta << Qt::noforcesign << " ms) pos " << p->position
          << ". Buffer: " << p->data_begin << " to " << p->data_end;
        out.flush();
        qInfo().noquote().nospace() << s;
      break;
    }
    default:
      qWarning() << "Unknown message code " << msg->messageCode;
  }
}

void DeviceInterface::dumpInboundPacket_(const QByteArray* packet) {
  if (!kDumpIncomingPackets) {
    return;
  }
  auto data = reinterpret_cast<const uint8_t*>(packet->constData());
  QString s{};
  auto out = QTextStream{&s};
  out.setIntegerBase(16);
  out.setFieldAlignment(QTextStream::AlignRight);
  for (int i = 0; i < (packet->size() / 2); ++i) {
    out.setFieldWidth(2);
    out.setPadChar('0');
    out << data[i * 2] << data[i * 2 + 1];
    out.setFieldWidth(0);
    out << (i % 8 == 7 ? "\n" :" ");
  }
  out.flush();
  //s.resize(s.size() - 1); // chomp;
  qInfo().noquote().nospace() << s;
}

void DeviceInterface::processStatusReply_(QByteArray* payload) {
  if (deviceStatus_ != payload->at(1)) {
    rx = true;
  }
  if (payload->size() < 6) {
    qInfo() << "Received status packet runt, length: " << payload->size();
    return;
  }
  device_status_t reply;
  memcpy(reply.raw, payload->data() + 1, sizeof(reply.raw));
  deviceStatus_ = reply.status;
  scanEnabled = deviceStatus_ & (1 << C2DEVSTATUS_SCAN_ENABLED);
  outputEnabled = deviceStatus_ & (1 << C2DEVSTATUS_OUTPUT_ENABLED);
  setupMode = deviceStatus_ & (1 << C2DEVSTATUS_SETUP_MODE);
  matrixMonitor = deviceStatus_ & (1 << C2DEVSTATUS_TELEMETRY_MODE);
  controllerInsane = deviceStatus_ & (1 << C2DEVSTATUS_INSANE);
  firmwareMajor = reply.versionMajor;
  firmwareMinor = reply.versionMinor;
  firmwareVersion = QString("%1.%2").arg(firmwareMajor).arg(firmwareMinor);
  dieTemp = QString("%1%2").arg((reply.dieTempSign == 1 ? '+' : '-')).arg(reply.dieTemp);
  prevDeviceTime = deviceTime;
  deviceTime = reply.sysTime;
  auto now = QDateTime::currentDateTime().toMSecsSinceEpoch();
  int64_t deviceDiff = deviceTime - prevDeviceTime;
  auto hostDiff = now - prevSysTime;
  if (deviceDiff < 0 && prevDeviceTime < (UINT32_MAX - 10000)) {
    // suppress crash detection if too close to rollover
    qWarning() << "Controller crash detected" << deviceTime << "ms ago";
  }
  prevDeviceTime = deviceTime;
  prevSysTime = now;
  deviceDrift = int((float(deviceDiff) - float(hostDiff)) * 1000 / hostDiff);
  // qInfo() << "dH:" << hostDiff << "dD:" << deviceDiff << "Drift, promille:" << deviceDrift;

  if (matrixMonitor) {
    setupMode = false;
  }
  emit notify(StatusUpdated);
}

/**
 * This is the handler of last resort for messages from device.
 * Other modules are supposed to install the event filter and process messages
 * of interest. Default behavior is to log them as strings.
 */
bool DeviceInterface::event(QEvent *e) {
  if (e->type() != DeviceMessage::ET) {
    return QObject::event(e);
  }
  QByteArray *payload = static_cast<DeviceMessage *>(e)->getPayload();
  switch (payload->at(0)) {
    case C2RESPONSE_TELEMETRY_ROW:
      break; // We are not interested in this, but it has >1 subscribers
    case C2RESPONSE_STATUS:
      processStatusReply_(payload);
      break;
    case C2RESPONSE_CODED_MESSAGE:
      decodeMessage_(*payload);
      dumpInboundPacket_(payload);
      break;
    case C2RESPONSE_LOG_MESSAGE: {
      auto msg = reinterpret_cast<const log_message_t*>(payload->constData());
      qInfo().noquote().nospace() << formatSysTime_(msg->sysTime)
                                  << msg->message;
      dumpInboundPacket_(payload);
      break;
    }
    default:
      qInfo() << payload->constData();
  }
  return true;
}

void DeviceInterface::enqueueCommand_(OUT_c2packet_t outbox) {
  if (state_ != DeviceConnected && state_ != BootloaderConnected) {
    qInfo() << "Command while disconnected";
    return;
  }
  {
    std::lock_guard<std::mutex> lock{queueLock_};
    outbox_.push(std::move(outbox));
  }
  resetTimer_(kHaveDataTickMs);
}

void DeviceInterface::sendCommand(c2command cmd, uint8_t *msg) {
  OUT_c2packet_t outbox;
  outbox.command = cmd;
  memcpy(outbox.payload, msg, sizeof(outbox.payload));
  enqueueCommand_(outbox);
}

void DeviceInterface::sendCommand(c2command cmd, uint8_t msg) {
  OUT_c2packet_t outbox;
  outbox.command = cmd;
  outbox.payload[0] = msg;
  enqueueCommand_(outbox);
}

void DeviceInterface::sendCommand(OUT_c2packet_t cmd) {
  enqueueCommand_(cmd);
}

void DeviceInterface::sendCommand(Bootloader_packet_t packet) {
  OUT_c2packet_t outbox;
  // Structure is marker+[payload]+len16+checksum16+marker
  memcpy(outbox.raw, packet.raw, packet.length + 7);
  enqueueCommand_(outbox);
}

void DeviceInterface::configLoaded() {
  qInfo() << "Configuration loaded.";
  if (state_ == DeviceConnected) {
    emit notify(DeviceConfigured);
  }
}

void DeviceInterface::bootloaderMode(bool bEnabled) {
  if (bEnabled) {
    qInfo() << "Entering firmware update mode.";
    mode_ = DeviceInterfaceBootloader;
    if (state_ == DeviceConnected) {
      sendCommand(C2CMD_ENTER_BOOTLOADER, 1);
    }
  } else {
    qInfo() << "Resuming normal operations.";
    mode_ = DeviceInterfaceNormal;
  }
  resetTimer_(kDeviceScanTick);
  scheduleDeviceRelease_.store(true);
}

void DeviceInterface::flipStatusBit(deviceStatus bit) {
  auto newStatus = deviceStatus_;
  auto oldValue = newStatus & (1 << bit);
  newStatus &= ~(1 << bit);
  if (!oldValue) {
    newStatus += (1 << bit);
  }
  sendCommand(C2CMD_EWO, newStatus);
}

void DeviceInterface::setStatusBit(deviceStatus bit, bool newValue) {
  auto newStatus = deviceStatus_;
  auto oldValue = newStatus & (1 << bit);
  if (oldValue != newValue) {
    flipStatusBit(bit);
  }
}

bool DeviceInterface::getStatusBit(deviceStatus bit) {
  return deviceStatus_ & (1 << bit);
}

void DeviceInterface::resetTimer_(int interval) {
  if (pollTimerInterval_ == interval) {
    return;
  }
  pollTimerInterval_ = interval;
  if (pollTimerId_ > 0) {
    killTimer(pollTimerId_);
  }
  qDebug() << "New poll timer interval:" << pollTimerInterval_;
  pollTimerId_ = startTimer(pollTimerInterval_, Qt::PreciseTimer);
}

void DeviceInterface::timerEvent(QTimerEvent* event) {
  if (event->timerId() == statusTimerId_) {
    if (mode_ == DeviceInterfaceNormal && config.bValid) {
      // request status, so you see actual status.
      sendCommand(C2CMD_GET_STATUS, 1);
    }
    return;
  }
  if (event->timerId() != pollTimerId_) {
    return;
  }
  if (!device_) {
    return initDevice_();
  }
  if (receivePacket_() || sendPacket_() || mode_ == DeviceInterfaceBootloader) {
    antiLagTimer_ = kAntiLagTicks;
  } else {
    // Go back to slow mode_ if idle and not bootloader.
    if (--antiLagTimer_ == 0) {
      qDebug("Idle. Slowing down to save CPU");
      resetTimer_(kNormalOperationTick);
      antiLagTimer_ = kAntiLagTicks;
    }
  }
  releaseDeviceIfScheduled_(); // handle device loss
}

bool DeviceInterface::sendRawPacket_DANGER_(const std::vector<uint8_t>& buf) {
  if (kDebugUsbTraffic) {
    qInfo() << "USB TX:" << buf[0] << buf[1] << buf[2] << "len" << packetSize;
  }
  if (hid_write(device_, buf.data(), packetSize + 1) == -1) {
    printHidError(device_);
    qWarning() << "Error sending to the device, will reconnect in 1 second";
    QThread::sleep(1);
    scheduleDeviceRelease_.store(true);
    return false;
  }
  return true;
}

bool DeviceInterface::sendPacket_() {
  OUT_c2packet_t cmd;
  bool shouldRetry{false};
  {
    std::lock_guard<std::mutex> lock{queueLock_};
    if (outbox_.empty()) {
      return false;
    }
    if (!device_) {
      UploadQueue tmp{};
      outbox_.swap(tmp);
      qWarning() << "Device went away on send";
      return true;
    }
    noCtsDelay_ -= pollTimerInterval_;
    if (cts_.exchange(false) == false && noCtsDelay_ > 0) {
      return true; // Do not slow down, may receive reply anytime soon!
    }
    noCtsDelay_ = kNoCtsDelayMs; // reset timer
    // Prevent deadlock - release queueLock_ first, _then_ acquire deviceLock_!
    shouldRetry = (--retriesLeft_ > 0);
    if (!shouldRetry) {
      cmd = outbox_.front();
      outbox_.pop();
    }
  }
  if (shouldRetry) {
    // Make sure this is a flip of condition above!
    std::lock_guard<std::mutex> lock{deviceLock_};
    return sendRawPacket_DANGER_(outgoingPacket_);
  }
  retriesLeft_ = kMaxPacketSendRetries;
  outgoingPacket_.clear();
  outgoingPacket_.reserve(packetSize);
  outgoingPacket_.emplace_back(kNoReport); // ReportID is not used.
  if (cmd.command != C2CMD_GET_STATUS) {
    tx = true;
    emit notify(StatusUpdated); // Blink the TX light
    lastSentAt_ = QDateTime::currentMSecsSinceEpoch();
  }
  for (size_t i = 0; i < sizeof(cmd); ++i) {
    outgoingPacket_.emplace_back(cmd.raw[i]);
  }
  if (kDebugUsbTraffic) {
    qInfo() << "Sending cmd " << cmd.command << (uint8_t)cmd.payload[0];
  }
  std::lock_guard<std::mutex> lock{deviceLock_};
  return sendRawPacket_DANGER_(outgoingPacket_);
}

bool DeviceInterface::receivePacket_() {
  std::vector<uint8_t> buffer{};
  buffer.resize(packetSize, 0);

  int bytesRead;
  {
    std::lock_guard<std::mutex> lock{deviceLock_};
    bytesRead = hid_read(device_, buffer.data(), buffer.size());
  }
  if (bytesRead == 0) {
    return false;
  } else if (bytesRead < 0) {
    qWarning() << "Device went away. Reconnecting";
    scheduleDeviceRelease_.store(true);
    return true; // block sending
  }
  if (kDebugUsbTraffic) {
    qInfo() << "USB rcv" << bytesRead << "b, cmd" << buffer.front() << "/" << buffer[1];
  }
  buffer.resize(bytesRead);
  cts_.store(true);
  retriesLeft_ = kAbsolutelyNoRetriesLeft;
  if (buffer.front() != C2RESPONSE_STATUS) {
    rx = true;
    if (lastSentAt_ > 0) {
      latencyMs = QString("%1 ms ").arg(
          QDateTime::currentMSecsSinceEpoch() - lastSentAt_);
      lastSentAt_ = 0;
    }
  }
  QCoreApplication::postEvent(this, new DeviceMessage(buffer));
  return rx;
}

void DeviceInterface::initDevice_() {
  config.reset(0);
  std::lock_guard<std::mutex> lock{deviceLock_};
  device_ = acquireDevice_();
  if (!device_) {
    qInfo() << ".";
    resetTimer_(kDeviceScanTick);
    return;
  }
  // auto info = hid_get_device_info(device_);
  hid_set_nonblocking(device_, 1);
  // We want to put device into setup mode AND trigger scanner init on connect.
  if (mode_ == DeviceInterfaceBootloader) {
    packetSize = 65; // This is for standard Cypress USB bootloader only
    setState_(BootloaderConnected);
  } else {
    std::vector<uint8_t> buf{kNoReport, C2CMD_EWO};
    // change above to C2CMD_ENTER_BOOTLOADER to put directly into bootloader.
    // THIS IS DANGEROUS - MAKE SURE YOU HAVE ANOTHER KEYBOARD CONNECTED!
    buf.emplace_back(1 << C2DEVSTATUS_SETUP_MODE);
    if (!sendRawPacket_DANGER_(buf)) {
      return; // Check if we can send, determine MTU
    }
    auto tmp = std::vector<uint8_t>(C2_MTU);
    auto bytesRead = hid_read_timeout(device_, tmp.data(), tmp.size(), 1000);
    if (bytesRead < 1) {
      if (bytesRead == -1) {
        printHidError(device_);
      }
      qCritical() << "Cannot get response from the device, will reconnect..";
      scheduleDeviceRelease_.store(true);
      return;
    }
    packetSize = bytesRead;
    qInfo() << "Transport MTU is" << packetSize;
    config.reset(packetSize);

    // Only flipping of the scan bit reinits scanner.
    // We do this on connect to reset transient insanity state (if any).
    // Probably not a good idea diagnostically - but let it stay for now.
    buf.back() += 1 << deviceStatus::C2DEVSTATUS_SCAN_ENABLED;
    if (!sendRawPacket_DANGER_(buf)) {
      return;
    }
    setState_(DeviceConnected);
  }

  cts_.store(true); // Not expecting anything from device - clear to send.
  // No need to change timer rate - loading config will switch it.
  return;
}

void DeviceInterface::setState_(State newState) {
  if (newState != state_) {
    state_ = newState;
    emit notify(state_);
  }
}

void DeviceInterface::updateThresholds() {
  emit notify(ThresholdsUpdated);
}

DeviceInterface::DetectedDevices DeviceInterface::listDevices_() {
  DetectedDevices retval{};
  auto root = hid_enumerate(0, 0);
  if (!root) {
    qInfo() << "No HID devices on this system?";
    return retval;
  }
  auto d{root};
  while (d) {
//        qInfo() << d->path << d->vendor_id << d->product_id;
// Usage and usage page are win and mac only :(
#ifdef __linux__
    if (d->vendor_id == 0x4114 && d->interface_number == 1) {
#else
    if (d->usage_page == 0x6213 && d->usage == 0x88) {
#endif
      if (d->bus_type == HID_API_BUS_USB) {
      auto devname = QString();
      devname += d->bus_type == HID_API_BUS_USB ? "[USB] " : "[BLE] ";
      devname += QString::fromWCharArray(d->serial_number);
      retval.keyboards.push_back(std::make_pair(devname, d->path));
      }
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

hid_device* DeviceInterface::acquireDevice_() {
  if (hid_init()) {
    qInfo() << "Cannot initialize hidapi!";
    return nullptr;
  }
  config.reset(0);
  auto devices = listDevices_();
  if (mode_ == DeviceInterfaceNormal && !devices.bootloaders.empty()) {
    QMessageBox::StandardButton result =
        QMessageBox::question(nullptr, "Bootloader detected!",
                              "Detected an active bootloader. "
                              "Do you want to update that device's firmware?",
                              QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::Yes) {
      mode_ = DeviceInterfaceBootloader;
    }
  }

  const auto& what = mode_ == DeviceInterfaceNormal ? devices.keyboards
                                                    : devices.bootloaders;
  if (what.empty()) {
    return nullptr;
  }
  if (mode_ == DeviceInterfaceNormal && !lastConnectedDevice.isEmpty()) {
    qInfo() << "Attempting to reconnect to" << lastConnectedDevice;
    for (unsigned int i = 0; i < what.size(); i++) {
      if (what.at(i).first == lastConnectedDevice) {
        return openDevice_(what.at(i));
      }
    }
    return nullptr;
  } else if (what.size() == 1) {
    qInfo() << "Found a node!";
    qDebug() << "Trying to use" << what.at(0).second.data();
    return openDevice_(what.at(0));
  }
  // More than one device.
  qInfo() << "Hello, fellow DT member!";
  DeviceSelector selector{what};
  selector.exec();
  auto deviceSerial = selector.getResult();
  qInfo().noquote() << "Connecting to s/n" << deviceSerial;
  for (const auto& it : what) {
    if (it.first == deviceSerial) {
      return openDevice_(it);
    }
  }
  return nullptr;
}

hid_device* DeviceInterface::openDevice_(const DeviceConnectionParams& params) {
  if (mode_ == DeviceInterfaceNormal) {
    // prevent post-flash reconnect - bootloader has different serial!
    lastConnectedDevice = params.first;
  }
  return hid_open_path(params.second.data());
}

void DeviceInterface::releaseDeviceIfScheduled_() {
  if (!scheduleDeviceRelease_.exchange(false)) {
    retriesLeft_ = kAbsolutelyNoRetriesLeft;
    return;
  }
  std::lock_guard<std::mutex> lock{deviceLock_};
  if (device_) {
    qInfo() << "Releasing device..";
  }
  setState_(DeviceDisconnected);
  hid_close(device_);
  device_ = nullptr;
  if (hid_exit() != 0) {
    qWarning("warning: error during hid_exit");
  }
}

void DeviceInterface::start() {
  qInfo() << "Acquiring device - can take a while with certain old USB hubs..";
  resetTimer_(kNormalOperationTick);
}
