/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <QApplication>
#include "DeviceInterface.h"
#include "FlightController.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  QCoreApplication::setOrganizationDomain("none.exists");
  QCoreApplication::setOrganizationName("DMA Labs");
  QCoreApplication::setApplicationName("FlightController");
  QCoreApplication::setApplicationVersion("1.0.2.1");
  qInfo() << "Start start!";
  FlightController w{true};
  w.show();

  return a.exec();
}
