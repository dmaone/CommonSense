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
    StatusUpdated
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
    scheduleDeviceRelease_.store(true);
  };

  DeviceConfig config;
  bool scanEnabled{false};
  bool outputEnabled{false};
  bool setupMode{false};
  bool matrixMonitor{false};
  bool controllerInsane{false};
  std::atomic<bool> rx{false};
  std::atomic<bool> tx{false};
  uint8_t firmwareMajor{0};
  uint8_t firmwareMinor{0};
  QString firmwareVersion{};
  QString dieTemp{};
  QString latencyMs{};
  uint8_t packetSize{0};  // BLE uses 130-byte packets, USB - 64-byte

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
  void resetTimer_(int interval);

  hid_device* device_{nullptr};
  int pollTimerId_{0};
  int statusTimerId_{0};
  Mode mode_{DeviceInterfaceNormal};
  State state_{DeviceDisconnected};
  uint8_t deviceStatus_{0};
  UploadQueue outbox_{};
  std::atomic<bool> cts_{true};
  size_t noCtsDelay_{0};
  size_t antiLagTimer_{0};
  std::atomic<bool> scheduleDeviceRelease_{false};
  std::mutex deviceLock_{};
  std::mutex queueLock_{};
  qint64 lastSentAt_;
};
