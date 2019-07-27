/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "FlightController.h"
#include <QApplication>

static LogViewer *logger;
static QtMessageHandler old_logger;

void logToViewport(QtMsgType type, const QMessageLogContext &context,
                   const QString &msg) {
  if (logger) {
    if (msg.length() < 3) {
      logger->continueMessage(msg);
    } else {
      logger->logMessage(msg);
    }
  } else if (old_logger) {
    old_logger(type, context, msg);
  }
}

int main(int argc, char *argv[]) {
  logger = NULL;
  old_logger = NULL;
  QApplication a(argc, argv);
  QCoreApplication::setOrganizationDomain("none.exists");
  QCoreApplication::setOrganizationName("DMA Labs");
  QCoreApplication::setApplicationName("FlightController");
  QCoreApplication::setApplicationVersion("1.0.0.0");
  FlightController w;
  logger = w.getLogViewport();
  old_logger = qInstallMessageHandler(logToViewport);
  w.setOldLogger(&old_logger);
  w.setup();
  w.show();

  return a.exec();
}
