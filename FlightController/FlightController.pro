#-------------------------------------------------
#
# CommonSense project - FlightController utility.
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = FlightController
TEMPLATE = app

CONFIG += static

# Not needed for linux, but doesn't hurt.
INCLUDEPATH += ../hidapi

win32 {
    LIBS += -L$$PWD/../hidapi/windows/.libs -lhidapi -lsetupapi
    RC_FILE = WindowsIcon.rc
}
macx {
    LIBS += -L$$PWD/../hidapi/mac/.libs -lhidapi -rpath @executable_path/../Frameworks
    ICON = FlightController.icns
}
linux {
    LIBS += -lhidapi-hidraw
}

SOURCES += main.cpp \
    FlightController.cpp \
    LogViewer.cpp \
    DeviceInterface.cpp \
    Events.cpp \
    MatrixMonitor.cpp \
    LayoutEditor.cpp \
    ScancodeList.cpp \
    ThresholdEditor.cpp \
    DeviceConfig.cpp \
    LayerConditions.cpp \
    FirmwareLoader.cpp \
    CyACD.cpp \
    LayerCondition.cpp \
    Delays.cpp

HEADERS  += \
    ../c2/c2_protocol.h \
    ../c2/nvram.h \
    call_once.h \
    singleton.h \
    settings.h \
    FlightController.h \
    LogViewer.h \
    DeviceInterface.h \
    Events.h \
    MatrixMonitor.h \
    LayoutEditor.h \
    ScancodeList.h \
    ThresholdEditor.h \
    DeviceConfig.h \
    LayerConditions.h \
    FirmwareLoader.h \
    CyACD.h \
    LayerCondition.h \
    Delays.h

FORMS    += \
    FlightController.ui \
    MatrixMonitor.ui \
    LayoutEditor.ui \
    ThresholdEditor.ui

DISTFILES +=
