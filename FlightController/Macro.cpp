#include "Macro.h"
#include <QDebug>

Macro::Macro(
    const uint8_t keyCode, const uint8_t flags, const QByteArray& body):
    keyCode(keyCode), flags(flags), body(body) {
}

QByteArray Macro::toBin() {
  QByteArray retval;
  retval.append(keyCode);
  retval.append(flags);
  retval.append(static_cast<uint8_t>(body.length()));
  retval.append(body);
  return retval;
}
