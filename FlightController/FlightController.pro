#-------------------------------------------------
#
# CommonSense project - FlightController utility.
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FlightController
TEMPLATE = app


win32 {
    LIBS += -L$$PWD/../hidapi/windows/Debug/ -lhidapi -lsetupapi
    CONFIG += static
    INCLUDEPATH += ../hidapi/hidapi
    SOURCES += ../hidapi/windows/hid.c
}

SOURCES += main.cpp \
    FlightController.cpp \
    LogViewer.cpp \
    DeviceInterface.cpp \
    Events.cpp \
    MatrixMonitor.cpp \
    LayoutEditor.cpp \
    ScancodeList.cpp \
    ThresholdEditor.cpp

HEADERS  += \
    FlightController.h \
    LogViewer.h \
    DeviceInterface.h \
    Events.h \
    call_once.h \
    singleton.h \
    ../c2/c2_protocol.h \
    MatrixMonitor.h \
    ../c2/nvram.h \
    LayoutEditor.h \
    ScancodeList.h \
    ThresholdEditor.h

FORMS    += \
    FlightController.ui \
    MatrixMonitor.ui \
    LayoutEditor.ui \
    ThresholdEditor.ui

DISTFILES +=
