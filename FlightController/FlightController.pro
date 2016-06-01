#-------------------------------------------------
#
# Project created by QtCreator 2016-05-29T16:37:03
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
    Events.cpp

HEADERS  += \
    FlightController.h \
    LogViewer.h \
    DeviceInterface.h \
    Events.h \
    call_once.h \
    singleton.h

FORMS    += \
    FlightController.ui
