#-------------------------------------------------
#
# Project created by QtCreator 2013-07-22T20:19:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport

TARGET = QkDaemon
TEMPLATE = app

INCLUDEPATH += core
INCLUDEPATH += gui
INCLUDEPATH += ../QkLib

QMAKE_LIBDIR += ../QkLib/build/release
LIBS += -lQk
#LIBS += -lQk

SOURCES += main.cpp\
        gui/mainwindow.cpp \
    core/qkdaemon.cpp \
    core/qkconnect.cpp \
    gui/qkexplorerwidget.cpp \
    gui/qkdaemonwidget.cpp \
    gui/qkconnectwidget.cpp \
    gui/ptablewidget.cpp \
    gui/ptoolbutton.cpp \
    gui/qkrawwidget.cpp

HEADERS  += gui/mainwindow.h \
    core/qkdaemon.h \
    core/qkconnect.h \
    gui/qkexplorerwidget.h \
    gui/qkdaemonwidget.h \
    gui/qkconnectwidget.h \
    gui/ptablewidget.h \
    gui/ptoolbutton.h \
    gui/qkrawwidget.h

FORMS    += gui/mainwindow.ui \
    gui/qkexplorerwidget.ui \
    gui/qkdaemonwidget.ui \
    gui/qkconnectwidget.ui \
    gui/qkrawwidget.ui

RESOURCES += \
    shared/icons/icons.qrc

CONFIG(debug, debug_and_release) {
    DESTDIR = build/debug
} else {
    DESTDIR = build/release
}

OBJECTS_DIR = build/tmp/obj
MOC_DIR = build/tmp/moc
RCC_DIR = build/tmp/rcc
UI_DIR = build/tmp/ui

