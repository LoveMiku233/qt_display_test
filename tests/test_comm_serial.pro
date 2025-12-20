QT += core
CONFIG += console c++11
CONFIG -= app_bundle
TEMPLATE = app
TARGET = test_comm_serial

INCLUDEPATH += $$PWD/..

SOURCES += \
    test_comm_serial.cpp \
    ../comm/comm_serial.cpp \
    ../comm/base/comm_adapter.cpp

HEADERS += \
    ../comm/comm_serial.h \
    ../comm/base/comm_adapter.h
