/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#pragma once

#include <QDateTime>
#include <QObject>
#include <hidapi/hidapi.h>
#include <mutex>
#include <queue>

#include "DeviceConfig.h"
#include "DeviceSelector.h"

class DeviceInterface : public QObject {
  Q_OBJECT

 public:
  enum State {
    DeviceDisconnected,
    BootloaderConnected,
    DeviceConnected,
    DeviceConfigured,
    StatusUpdated,
    ThresholdsUpdated,
  };
  Q_ENUM(State)

  enum KeyState { KeyPressed, KeyReleased };
  Q_ENUM(KeyState)

  enum Mode { DeviceInterfaceNormal, DeviceInterfaceBootloader };
  Q_ENUM(Mode)

  DeviceInterface();
  ~DeviceInterface();
  void start();
  bool event(QEvent *e);
  device_status_t *getStatus();
  void scheduleDeviceRelease() {
    lastConnectedDevice.clear();
    scheduleDeviceRelease_.store(true);
  };
  void updateThresholds();

  DeviceConfig config;
  QString lastConnectedDevice{};
  bool scanEnabled{false};
  bool outputEnabled{false};
  bool setupMode{false};
  bool matrixMonitor{false};
  bool controllerInsane{false};
  std::atomic<bool> rx{false};
  std::atomic<bool> tx{false};
  uint8_t firmwareMajor{0};
  uint8_t firmwareMinor{0};
  uint32_t deviceTime{0};
  uint32_t prevDeviceTime{0};
  uint64_t prevSysTime{0};
  int32_t deviceDrift{0};
  QString firmwareVersion{};
  QString dieTemp{};
  QString latencyMs{};
  uint8_t packetSize{64};  // BLE uses 130-byte packets, USB - 64-byte

 public slots:
  void sendCommand(c2command, uint8_t *);
  void sendCommand(c2command, uint8_t);
  void sendCommand(OUT_c2packet_t);
  void sendCommand(Bootloader_packet_t);

  bool getStatusBit(deviceStatus bit);
  void setStatusBit(deviceStatus bit, bool value);
  void flipStatusBit(deviceStatus bit);
  void configLoaded();
  void bootloaderMode(bool bEnable);

 signals:
  void notify(DeviceInterface::State);
  void keypress(uint8_t keyIndex, DeviceInterface::KeyState state);
  void pedal(uint8_t pedalIndex, DeviceInterface::KeyState state);

 protected:
  virtual void timerEvent(QTimerEvent* event);

 private:
  using UploadQueue = std::queue<OUT_c2packet_t>;
  struct DetectedDevices {
    DeviceList keyboards{};
    DeviceList bootloaders{};
  };

  hid_device* acquireDevice_();
  hid_device* openDevice_(const DeviceConnectionParams& params);
  void initDevice_();
  void releaseDeviceIfScheduled_();
  void setState_(State newState);
  bool receivePacket_();
  bool sendPacket_();
  bool sendRawPacket_DANGER_(const std::vector<uint8_t>& buf);

  void dumpInboundPacket_(const QByteArray* packet);
  void decodeMessage_(const QByteArray& payload);
  QString formatKey_(uint8_t keyIndex, uint8_t flags) const;
  QString formatSysTime_(uint32_t sysTime) const;
  void enqueueCommand_(OUT_c2packet_t outbox);
  DetectedDevices listDevices_();
  void processStatusReply_(QByteArray* payload);
  void killMainTimer_();
  void resetMainTimer_(int interval);

  hid_device* device_{nullptr};
  int mainTimerId_{0};
  int mainTimerInterval_{4114};
  int statusTimerId_{0};
  Mode mode_{DeviceInterfaceNormal};
  State state_{DeviceDisconnected};
  uint8_t deviceStatus_{0};
  UploadQueue outbox_{};
  std::vector<uint8_t> outgoingPacket_{};
  int retriesLeft_{0};
  std::atomic<bool> cts_{true};
  int64_t noCtsDelay_{0};
  size_t antiLagTimer_{0};
  std::atomic<bool> scheduleDeviceRelease_{false};
  std::mutex deviceLock_{};
  std::mutex queueLock_{};
  qint64 lastSentAt_;
};
