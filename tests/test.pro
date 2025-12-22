QT += core
CONFIG += console c++11
CONFIG -= app_bundle
TEMPLATE = app
TARGET = tests

INCLUDEPATH += $$PWD/..

SOURCES += \
    ../utils/utils.cpp\
    test_main.cpp \
    ../comm/comm_serial.cpp \
    ../comm/comm_can.cpp \
    ../comm/base/comm_adapter.cpp

HEADERS += \
    ../utils/utils.h \
    ../comm/comm_serial.h \
    ../comm/comm_can.h \
    ../comm/base/comm_adapter.h
