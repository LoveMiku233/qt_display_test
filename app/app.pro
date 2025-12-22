QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
TEMPLATE = app
TARGET = qt_display_yk_1

INCLUDEPATH += $$PWD/..

SOURCES += \
    ../main.cpp \
    ../pages/comm_page.cpp \
    ../pages/ctrl_page.cpp \
    ../pages/debug_page.cpp \
    ../pages/home_page.cpp \
    ../pages/log_page.cpp \
    ../pages/mainwindow.cpp \
    ../pages/setting_page.cpp \
    ../widget/card_widget.cpp

HEADERS += \
    ../pages/comm_page.h \
    ../pages/ctrl_page.h \
    ../pages/debug_page.h \
    ../pages/home_page.h \
    ../pages/log_page.h \
    ../pages/mainwindow.h \
    ../pages/setting_page.h \
    ../widget/card_widget.h

FORMS += \
    ../ui/ctrl_page.ui \
    ../ui/home_page.ui \
    ../ui/mainwindow.ui \
    ../ui/comm_page.ui \
    ../ui/log_page.ui \
    ../ui/setting_page.ui \
    ../ui/debug_page.ui

DISTFILES += \
    ../.gitignore

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
