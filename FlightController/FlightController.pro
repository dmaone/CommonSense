#-------------------------------------------------
#
# CommonSense project - FlightController utility.
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = FlightController
TEMPLATE = app

CONFIG += static c++17

CONFIG(release, debug|release) {
  DEFINES += QT_NO_DEBUG_OUTPUT
}

win32 {
    # mingw32 for release, mingw64 for debug (use dynamic, 64-bit kit!)
    INCLUDEPATH += $$PWD/../mingw32/include $$PWD/../mingw64/include
    LIBS += -L$$PWD/../mingw32/lib -L$$PWD/../mingw32/bin
    LIBS += -L$$PWD/../mingw64/lib -L$$PWD/../mingw64/bin
    LIBS += -lhidapi -lsetupapi
    RC_FILE = WindowsIcon.rc
}
macx {
    LIBS += -L$$PWD/../../hidapi/mac/.libs -lhidapi
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
    MatrixView.cpp \
    Pedals.cpp \
    ScancodeList.cpp \
    Thresholds.cpp \


HEADERS  += \
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
    MatrixView.h \
    Pedals.h \
    ScancodeList.h \
    settings.h \
    Thresholds.h \

FORMS    += \
    DeviceSelector.ui \
    FlightController.ui \
    Hardware.ui \
    Layout.ui \
    Macros.ui \
    MatrixView.ui \
    Pedals.ui \
    Thresholds.ui \

DISTFILES +=
