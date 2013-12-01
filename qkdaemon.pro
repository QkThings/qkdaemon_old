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
INCLUDEPATH += ../qkcore
INCLUDEPATH += ../qkapi/qt/qkapi

QMAKE_LIBDIR += ../qkcore/release
QMAKE_LIBDIR += ../qkapi/qt/qkapi/release
LIBS += -lqkcore
LIBS += -lqkapi

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
    core/qkdaemonsocket.cpp \
    core/qkapihandler.cpp \
    core/qkconnect.cpp \
    core/qkconnectthread.cpp \
    gui/boardpanel.cpp \
    gui/messagesdialog.cpp

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
    core/qkdaemonsocket.h \
    core/qkapihandler.h \
    core/qkconnect.h \
    core/qkconnectthread.h \
    gui/boardpanel.h \
    gui/messagesdialog.h \
    gui/gui_globals.h

FORMS    += gui/mainwindow.ui \
    gui/qkexplorerwidget.ui \
    gui/qkdaemonwidget.ui \
    gui/qkconnectwidget.ui \
    gui/qkrawwidget.ui \
    gui/settingsdialog.ui \
    gui/boardpanel.ui \
    gui/messagesdialog.ui

RESOURCES += \
    resources/icons/icons.qrc \
    resources/api/api.qrc \
    resources/img/img.qrc

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


