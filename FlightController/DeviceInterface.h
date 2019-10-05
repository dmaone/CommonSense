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
#include <../c2/c2_protocol.h>

#include "DeviceConfig.h"
#include "DeviceSelector.h"
#include "Events.h"
#include "LogViewer.h"
#include "singleton.h"

class DeviceInterface : public QObject {
  Q_OBJECT
  Q_ENUMS(DeviceStatus)
  Q_ENUMS(KeyStatus)
  Q_ENUMS(Mode)

public:
  static DeviceInterface& getInstance() {
    return Singleton<DeviceInterface>::instance();
  }
  static DeviceConfig* config;
  DeviceInterface(QObject *parent = 0);
  ~DeviceInterface();
  void start(void);
  bool event(QEvent *e);
  device_status_t *getStatus(void);
  void releaseDevice(void);
  enum DeviceStatus {
    DeviceConnected,
    DeviceDisconnected,
    DeviceConfigChanged,
    BootloaderConnected,
    StatusUpdated
  };
  enum KeyStatus { KeyPressed, KeyReleased };
  enum Mode { DeviceInterfaceNormal, DeviceInterfaceBootloader };
  bool scanEnabled {false};
  bool outputEnabled {false};
  bool setupMode {false};
  bool matrixMonitor {false};
  bool controllerInsane {false};
  bool printableStatus {false};
  bool rx {false};
  bool tx {false};
  QString switchType{};
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
  void configChanged(void);
  void bootloaderMode(bool bEnable);

signals:
  void deviceStatusNotification(DeviceInterface::DeviceStatus);
  void scancodeReceived(uint8_t row, uint8_t col,
                        DeviceInterface::KeyStatus status);

protected:
  virtual void timerEvent(QTimerEvent *);

private:
  hid_device *device;
  int pollTimerId;
  int statusTimerId;
  unsigned char bytesFromDevice[65];
  device_status_t status;
  uint8_t mode;
  DeviceStatus currentStatus;
  uint8_t receivedStatus_;
  QQueue<OUT_c2packet_t> commandQueue_;
  std::atomic<bool> cts_ {true};
  size_t noCtsDelay_;
  size_t antiLagTimer_;
  std::atomic<bool> releaseDevice_ {false};

  void processStatusReply(QByteArray* payload);
  hid_device *acquireDevice(void);
  void _initDevice(void);
  void _enqueueCommand(OUT_c2packet_t outbox);
  void _resetTimer(int interval);
  void _resetStatusTimer(int interval);
  bool _sendPacket(void);
  bool _receivePacket(void);
  void _updateDeviceStatus(DeviceStatus);
  DeviceList listDevices();
  DeviceConfig config_{};
  std::mutex deviceLock_{};
  std::mutex queueLock_{};
  qint64 lastSend_;

private slots:
  void deviceMessageReceiver(void);
};
