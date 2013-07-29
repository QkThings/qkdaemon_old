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

SOURCES += main.cpp\
        gui/mainwindow.cpp \
        gui/qkconnectwidget.cpp \
    core/qkdaemon.cpp \
    core/qkconnect.cpp \
    gui/qkexplorerwidget.cpp \
    gui/connectionstable.cpp

HEADERS  += gui/mainwindow.h \
        gui/qkconnectwidget.h \
    core/qkdaemon.h \
    core/qkconnect.h \
    gui/qkexplorerwidget.h \
    gui/connectionstable.h

FORMS    += ui/mainwindow.ui \
        ui/qkconnectwidget.ui \
    gui/qkexplorerwidget.ui

RESOURCES += \
    shared/icons/icons.qrc

CONFIG(debug, debug|release) {
    DESTDIR = build/debug
} else {
    DESTDIR = build/release
}

OBJECTS_DIR = build/tmp/obj
MOC_DIR = build/tmp/moc
RCC_DIR = build/tmp/rcc
UI_DIR = build/tmp/ui

