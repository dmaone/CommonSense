#pragma once

#include <stdint.h>
#include <QByteArray>

#include "../c2/c2_protocol.h"


class Macro
{
public:
  Macro(const uint8_t keyCode, const uint8_t flags, const QByteArray& body);
  Macro(const uint8_t keyCode,
        const QString& triggerEvent,
        const QByteArray& body);
  QByteArray toBin();
  QString fullName();
  QString getTriggerEventText();

  bool operator <(const Macro& other) const{
    // Tap macros must go first, as firmware depends on that order.
    return this->keyCode < other.keyCode &&
        (this->flags & MACRO_TYPE_TAP) > (other.flags & MACRO_TYPE_TAP);
  }

  uint8_t keyCode;
  uint8_t flags;
  QByteArray body;
};

