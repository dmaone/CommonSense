/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "LogViewer.h"
#include <QDateTime>
#include <QMessageBox>

LogViewer::LogViewer(QWidget *parent) : QPlainTextEdit(parent) {
  this->setReadOnly(true);
}

void LogViewer::logMessage(QString msg) {
  this->appendPlainText(QDateTime::currentDateTime().toString("hh:mm:ss.zzz "));
  this->insertPlainText(msg.replace("\n", "\n--:--:--.--- "));
  progress_ = 0;
  this->moveCursor(QTextCursor::End);
}

void LogViewer::continueMessage(QString msg) {
  if (msg != ".") {
    this->insertPlainText(msg);
    this->moveCursor(QTextCursor::End);
  } else {
    if (progress_ == 0) {
      this->insertPlainText(".. [0]");
    }
    // Progress indicator
    this->moveCursor(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
    this->moveCursor(QTextCursor::PreviousWord, QTextCursor::KeepAnchor);
    this->moveCursor(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
    this->insertPlainText(QString("[%1]").arg(progress_++));
  }
  repaint();
}

void LogViewer::clearAll() {
  QMessageBox::StandardButton result = QMessageBox::question(
      this, "Erase current logs", "Irreversibly! Ain't it a great thing to do?",
      QMessageBox::Yes | QMessageBox::No);
  if (result == QMessageBox::Yes)
    this->clear();
}

void LogViewer::copyAll() {
  this->selectAll();
  this->copy();
  QTextCursor cur = this->textCursor();
  cur.movePosition(cur.End);
  this->setTextCursor(cur);
}
