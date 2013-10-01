#-------------------------------------------------
#
# Project created by QtCreator 2013-07-22T20:19:20
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport

TARGET = QkDaemon
TEMPLATE = app

INCLUDEPATH += core
INCLUDEPATH += gui
INCLUDEPATH += gui/utils
INCLUDEPATH += ../QkLib
INCLUDEPATH += ../QkAPI

QMAKE_LIBDIR += ../QkLib/build/release
QMAKE_LIBDIR += ../QkAPI/build/release
LIBS += -lQk
LIBS += -lQkAPI

#CONFIG += extserialport

SOURCES += main.cpp\
        gui/mainwindow.cpp \
    core/qkdaemon.cpp \
    gui/qkexplorerwidget.cpp \
    gui/qkdaemonwidget.cpp \
    gui/qkconnectwidget.cpp \
    gui/utils/ptablewidget.cpp \
    gui/utils/ptoolbutton.cpp \
    gui/qkrawwidget.cpp \
    gui/utils/qhexspinbox.cpp \
    gui/utils/cpropertybrowser.cpp \
    gui/utils/cproperty.cpp \
    gui/utils/ptreeitemproperty.cpp \
    gui/settingsdialog.cpp \
    core/qkdaemonthread.cpp \
    core/qkdaemonserver.cpp \
    core/qkconnectionmanager.cpp \
    core/qkdaemonsocket.cpp \
    core/qkapihandler.cpp

HEADERS  += gui/mainwindow.h \
    core/qkdaemon.h \
    gui/qkexplorerwidget.h \
    gui/qkdaemonwidget.h \
    gui/qkconnectwidget.h \
    gui/utils/ptablewidget.h \
    gui/utils/ptoolbutton.h \
    gui/qkrawwidget.h \
    gui/utils/qhexspinbox.h \
    gui/utils/cpropertybrowser.h \
    gui/utils/cproperty.h \
    gui/utils/ptreeitemproperty.h \
    gui/settingsdialog.h \
    core/qkdaemonthread.h \
    core/qkdaemonserver.h \
    core/qkconnectionmanager.h \
    core/qkdaemonsocket.h \
    core/qkapihandler.h

FORMS    += gui/mainwindow.ui \
    gui/qkexplorerwidget.ui \
    gui/qkdaemonwidget.ui \
    gui/qkconnectwidget.ui \
    gui/qkrawwidget.ui \
    gui/settingsdialog.ui

RESOURCES += \
    shared/icons/icons.qrc \
    shared/api/api.qrc

CONFIG(debug, debug_and_release) {
    DESTDIR = build/debug
} else {
    DESTDIR = build/release
}

OBJECTS_DIR = build/tmp/obj
MOC_DIR = build/tmp/moc
RCC_DIR = build/tmp/rcc
UI_DIR = build/tmp/ui

