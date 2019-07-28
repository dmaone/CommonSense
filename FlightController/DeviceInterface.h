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
#include <QObject>
#include <QQueue>
#include <hidapi/hidapi.h>
#include <../c2/c2_protocol.h>

#include "DeviceConfig.h"
#include "Events.h"
#include "LogViewer.h"

class DeviceInterface : public QObject {
  Q_OBJECT
  Q_ENUMS(DeviceStatus)
  Q_ENUMS(KeyStatus)
  Q_ENUMS(Mode)

public:
  DeviceInterface(QObject *parent = 0);
  ~DeviceInterface();
  void start(void);
  bool event(QEvent *e);
  device_status_t *getStatus(void);
  void releaseDevice(void);
  DeviceConfig *config;
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
  QString firmwareVersion;
  QString dieTemp;

public slots:
  void sendCommand(c2command, uint8_t *);
  void sendCommand(c2command, uint8_t);
  void sendCommand(OUT_c2packet_t);
  void sendCommand(Bootloader_packet_t);

  // NEVER use this function - it's only for sending packet in close event
  // because timers don't work in close event!
  void sendCommandNow(c2command cmd, uint8_t);

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
  std::atomic<bool> releaseDevice_ {false};

  void processStatusReply(QByteArray* payload);
  hid_device *acquireDevice(void);
  void _initDevice(void);
  void _enqueueCommand(OUT_c2packet_t outbox);
  void _resetTimer(int interval);
  void _resetStatusTimer(int interval);
  void _sendPacket(void);
  void _receivePacket(void);
  void _updateDeviceStatus(DeviceStatus);
  std::vector<std::pair<QString, std::string>> listDevices();
  std::mutex queueLock_{};

private slots:
  void deviceMessageReceiver(void);
};
