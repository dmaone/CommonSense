/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#pragma once

#include <QPlainTextEdit>

class LogViewer : public QPlainTextEdit {
  Q_OBJECT

 public:
  LogViewer(QWidget *parent = NULL);

 public slots:
  void clearButtonClick();
  void copyAllButtonClick();
  void logMessage(QString msg);
  void continueMessage(QString msg);

 private:
  uint32_t progress_{0}; // for progress indicators
};
