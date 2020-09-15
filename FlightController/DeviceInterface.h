/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#pragma once

#include <mutex>
#include <QDateTime>
#include <QObject>
#include <QQueue>
#include <hidapi/hidapi.h>

#include "DeviceConfig.h"
#include "DeviceSelector.h"
#include "Events.h"
#include "LogViewer.h"

class DeviceInterface : public QObject {
  Q_OBJECT

 public:
  enum DeviceStatus {
    DeviceDisconnected,
    BootloaderConnected,
    DeviceConnected,
    DeviceConfigured,
    StatusUpdated
  };
  Q_ENUM(DeviceStatus)

  enum KeyStatus { KeyPressed, KeyReleased };
  Q_ENUM(KeyStatus)

  enum Mode { DeviceInterfaceNormal, DeviceInterfaceBootloader };
  Q_ENUM(Mode)

  struct KeyState{
    uint8_t row;
    uint8_t col;
    DeviceInterface::KeyStatus status;
  };

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
  QString firmwareVersion{};
  QString dieTemp{};
  QString latencyMs{};

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
  void deviceStatusNotification(DeviceInterface::DeviceStatus);
  void keypress(DeviceInterface::KeyState state);

 protected:
  virtual void timerEvent(QTimerEvent *);

 private:
  struct DetectedDevices {
    DeviceList keyboards{};
    DeviceList bootloaders{};
  };

  hid_device* acquireDevice();
  void releaseDevice_();

  void _initDevice();
  void _enqueueCommand(OUT_c2packet_t outbox);
  void _resetTimer(int interval);
  void _resetStatusTimer(int interval);
  bool _sendPacket();
  bool _receivePacket();
  void _updateDeviceStatus(DeviceStatus newStatus);
  DetectedDevices listDevices();
  void processStatusReply(QByteArray* payload);

  hid_device* device_{nullptr};
  int pollTimerId_{0};
  int statusTimerId_{0};
  device_status_t status{};
  Mode mode_{DeviceInterfaceNormal};
  DeviceStatus currentStatus_{DeviceDisconnected};
  uint8_t receivedStatus_{0};
  QQueue<OUT_c2packet_t> commandQueue_{};
  std::atomic<bool> cts_{true};
  size_t noCtsDelay_{0};
  size_t antiLagTimer_{0};
  std::atomic<bool> scheduleDeviceRelease_{false};
  std::mutex deviceLock_{};
  std::mutex queueLock_{};
  qint64 lastSend_;

 private slots:
  void deviceMessageReceiver();
};
