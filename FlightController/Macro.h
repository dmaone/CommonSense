#pragma once

#include <stdint.h>
#include <QByteArray>



class Macro
{
public:
  Macro(const uint8_t keyCode, const uint8_t flags, const QByteArray& body);
  QByteArray toBin();
  QString fullName();
  QString getTriggerEventText();

  uint8_t keyCode;
  uint8_t flags;
  QByteArray body;
};

