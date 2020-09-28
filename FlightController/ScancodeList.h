#pragma once
#include <QStringList>

class ScancodeList : public QStringList {
 public:
  ScancodeList() : list{getScancodeList_()} {};
  QStringList* list;
  int width{2}; // Not a real width - fiddle if something doesn't fit.

  static QString asString(uint8_t code);

 private:
  static QStringList* getScancodeList_();
};
