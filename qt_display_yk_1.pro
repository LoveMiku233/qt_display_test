QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    device/base/device_adapter.cpp \
    main.cpp \
    pages/comm_page.cpp \
    pages/ctrl_page.cpp \
    pages/home_page.cpp \
    pages/log_page.cpp \
    pages/mainwindow.cpp \
    pages/setting_page.cpp

HEADERS += \
    device/base/device_adapter.h \
    pages/comm_page.h \
    pages/ctrl_page.h \
    pages/home_page.h \
    pages/log_page.h \
    pages/mainwindow.h \
    pages/setting_page.h

FORMS += \
    ui/ctrl_page.ui \
    ui/home_page.ui \
    ui/mainwindow.ui \
    ui/comm_page.ui \
    ui/log_page.ui \
    ui/setting_page.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    .gitignore
