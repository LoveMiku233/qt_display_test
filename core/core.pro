QT += core network
CONFIG += console c++11
CONFIG -= app_bundle
TEMPLATE = app
TARGET = core_server

INCLUDEPATH += $$PWD/..

SOURCES += \
    ../utils/utils.cpp\
    ../comm/base/comm_adapter.cpp \
    ../comm/comm_serial.cpp \
    ../comm/comm_can.cpp \
    ../device/base/device_adapter.cpp \
    ../rpc/json_rpc_dispatcher.cpp \
    core_main.cpp

HEADERS += \
    ../utils/utils.h\
    ../comm/base/comm_adapter.h \
    ../comm/comm_serial.h \
    ../comm/comm_can.h \
    ../rpc/json_rpc_dispatcher.h \
    ../device/base/device_adapter.h \
