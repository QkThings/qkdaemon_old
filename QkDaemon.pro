#-------------------------------------------------
#
# Project created by QtCreator 2013-07-22T20:19:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QkDaemon
TEMPLATE = app

INCLUDEPATH += gui

SOURCES += main.cpp\
        gui/mainwindow.cpp \
        gui/qkconnectwidget.cpp

HEADERS  += gui/mainwindow.h \
        gui/qkconnectwidget.h

FORMS    += ui/mainwindow.ui \
        ui/qkconnectwidget.ui

CONFIG(debug, debug|release) {
    DESTDIR = build/debug
} else {
    DESTDIR = build/release
}

OBJECTS_DIR = build/tmp/obj
MOC_DIR = build/tmp/moc
RCC_DIR = build/tmp/rcc
UI_DIR = build/tmp/ui

OTHER_FILES +=

RESOURCES +=
