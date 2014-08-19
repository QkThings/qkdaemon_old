#-------------------------------------------------
#
# Project created by QtCreator 2013-07-22T20:19:20
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport printsupport

TARGET = qkdaemon
TEMPLATE = app

QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN/../shared/lib/\',-z,origin'

#DEFINES += QT_NO_DEBUG_OUTPUT

INCLUDEPATH += core
INCLUDEPATH += gui
INCLUDEPATH += gui/widgets
INCLUDEPATH += ../utils

INCLUDEPATH += ../qkcore
QMAKE_LIBDIR += ../qkcore/release
LIBS += -L../qkcore/release -lqkcore

INCLUDEPATH += ../qkapi/qt
QMAKE_LIBDIR += ../qkapi/qt/release
LIBS += -L../qkapi/qt/release -lqkapi

INCLUDEPATH += ../qkwidget
QMAKE_LIBDIR += ../qkwidget/release
LIBS += -L../qkwidget/release -lqkwidget

#CONFIG += extserialport

SOURCES += main.cpp\
        gui/mainwindow.cpp \
    core/qkdaemon.cpp \
    gui/qkdaemonwidget.cpp \
    gui/qkconnectwidget.cpp \
    gui/widgets/ptablewidget.cpp \
    gui/widgets/ptoolbutton.cpp \
    gui/settingsdialog.cpp \
    core/qkdaemonthread.cpp \
    core/qkdaemonserver.cpp \
    core/qkdaemonsocket.cpp \
    core/qkapihandler.cpp \
    gui/messagesdialog.cpp

HEADERS  += gui/mainwindow.h \
    core/qkdaemon.h \
    gui/qkdaemonwidget.h \
    gui/qkconnectwidget.h \
    gui/widgets/ptablewidget.h \
    gui/widgets/ptoolbutton.h \
    gui/settingsdialog.h \
    core/qkdaemonthread.h \
    core/qkdaemonserver.h \
    core/qkdaemonsocket.h \
    core/qkapihandler.h \
    gui/messagesdialog.h \
    gui/gui_globals.h

FORMS    += gui/mainwindow.ui \
    gui/qkdaemonwidget.ui \
    gui/qkconnectwidget.ui \
    gui/settingsdialog.ui \
    gui/messagesdialog.ui

RESOURCES += \
    resources/api/qkdaemon_api.qrc \
    resources/icons/qkdaemon_icons.qrc \
    resources/img/qkdaemon_img.qrc

#RC_ICONS = resources/img/qk_64.png

CONFIG(debug, debug|release) {
    DESTDIR = debug
} else {
    DESTDIR = release
}

OBJECTS_DIR = build/obj
MOC_DIR = build/moc
RCC_DIR = build/rcc
UI_DIR = build/ui

deploy.commands = python deploy.py

QMAKE_EXTRA_TARGETS += deploy
POST_TARGETDEPS += deploy
