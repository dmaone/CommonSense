#include "Macro.h"

#include <QDebug>

#include "ScancodeList.h"

Macro::Macro(
    const uint8_t keyCode, const uint8_t flags, const QByteArray& body):
    keyCode(keyCode), flags(flags), body(body) {
}

Macro::Macro(const uint8_t kc, const QString& trigger, const QByteArray& body) :
    keyCode(kc), body(body) {
  flags = 0;
  if (trigger == "release") {
    flags |= MACRO_TYPE_ONKEYUP;
  } else if (trigger == "tap") {
    flags |= MACRO_TYPE_TAP;
  } else if (trigger == "press") {
    // Do nothing, it's default
  } else {
    qInfo() << "Unknown trigger event: " << trigger;
  }
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
  QString fullName{scancodeList.list->at(keyCode)};
  fullName.append(" (");
  fullName.append(getTriggerEventText());
  fullName.append(")");
  return fullName;
}

QString Macro::getTriggerEventText() {
  if (flags & MACRO_TYPE_ONKEYUP) {
    return "release";
  } else if (flags & MACRO_TYPE_TAP) {
    return "tap";
  } else {
    return "press";
  }
}
