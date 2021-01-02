#-------------------------------------------------
#
# CommonSense project - FlightController utility.
#
#-------------------------------------------------

QT += core gui widgets

TARGET = FlightController
TEMPLATE = app

CONFIG += static c++17

CONFIG(release, debug|release) {
  DEFINES += QT_NO_DEBUG_OUTPUT
}

win32 {
  # mingw32 for release, mingw64 for debug (use dynamic, 64-bit kit!)
  INCLUDEPATH += $$PWD/../mingw32/include $$PWD/../mingw64/include
  CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../mingw64/lib -L$$PWD/../mingw64/bin
  }
  CONFIG(release, debug|release) {
    LIBS += -L$$PWD/../mingw32/lib -L$$PWD/../mingw32/bin
  }
  LIBS += -lhidapi -lsetupapi
  RC_FILE = WindowsIcon.rc
}
macx {
  INCLUDEPATH += /usr/local/include
  LIBS += -L/usr/local/lib -lhidapi
  ICON = FlightController.icns
}
linux {
  LIBS += -lhidapi-hidraw
}

SOURCES += main.cpp \
  CyACD.cpp \
  Delays.cpp \
  DeviceConfig.cpp \
  DeviceInterface.cpp \
  DeviceSelector.cpp \
  Events.cpp \
  FirmwareLoader.cpp \
  FlightController.cpp \
  Hardware.cpp \
  LayerCondition.cpp \
  LayerConditions.cpp \
  Layout.cpp \
  LogViewer.cpp \
  Macro.cpp \
  Macros.cpp \
  Pedals.cpp \
  ScancodeList.cpp \
  Telemetry.cpp \
  Thresholds.cpp \


HEADERS += \
  ../c2/nvram.h \
  ../c2/c2_protocol.h \
  CyACD.h \
  Delays.h \
  DeviceConfig.h \
  DeviceInterface.h \
  DeviceSelector.h \
  Events.h \
  FirmwareLoader.h \
  FlightController.h \
  Hardware.h \
  LayerCondition.h \
  LayerConditions.h \
  Layout.h \
  LogViewer.h \
  Macro.h \
  Macros.h \
  Pedals.h \
  ScancodeList.h \
  Telemetry.h \
  settings.h \
  Thresholds.h \

FORMS += \
  DeviceSelector.ui \
  FlightController.ui \
  Hardware.ui \
  Layout.ui \
  Macros.ui \
  Pedals.ui \
  Telemetry.ui \
  Thresholds.ui \
