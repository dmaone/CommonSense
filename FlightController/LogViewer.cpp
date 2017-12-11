/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "LogViewer.h"
#include <QMessageBox>

LogViewer::LogViewer(QWidget *parent) : QPlainTextEdit(parent) {
  this->setReadOnly(true);
}

void LogViewer::logMessage(QString msg) {
  this->appendPlainText(msg);
  repaint();
}

void LogViewer::continueMessage(QString msg) {
  this->moveCursor(QTextCursor::End);
  this->insertPlainText(msg);
  this->moveCursor(QTextCursor::End);
  repaint();
}

void LogViewer::clearButtonClick(void) {
  QMessageBox::StandardButton result = QMessageBox::question(
      this, "Erase current logs", "Irreversibly! Ain't it a great thing to do?",
      QMessageBox::Yes | QMessageBox::No);
  if (result == QMessageBox::Yes)
    this->clear();
}

void LogViewer::copyAllButtonClick(void) {
  this->selectAll();
  this->copy();
  QTextCursor cur = this->textCursor();
  cur.movePosition(cur.End);
  this->setTextCursor(cur);
}
