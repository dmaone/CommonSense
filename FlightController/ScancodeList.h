#pragma once
#include <QStringList>

class ScancodeList : public QStringList {
 public:
  ScancodeList() : list{_getScancodeList()} {};
  QStringList* list;
  int width{2}; // Not a real width - fiddle if something doesn't fit.

 private:
  static QStringList* _getScancodeList();
};
