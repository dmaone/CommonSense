#include "Macro.h"

#include <QDebug>
#include "../../c2/c2_protocol.h"

#include "ScancodeList.h"

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

QString Macro::fullName() {
  ScancodeList scancodeList;
  QString fullName{scancodeList.list[keyCode]};
  if (flags && MACRO_TYPE_TAP) {
    fullName.append(" (release)");
  } else if (flags && MACRO_TYPE_TAP) {
    fullName.append(" (tap)");
  } else {
    fullName.append(" (press)");
  }
  return fullName;
}
