#-------------------------------------------------
#
# Project created by QtCreator 2013-07-22T20:19:20
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport printsupport

TARGET = QkDaemon
TEMPLATE = app

INCLUDEPATH += core
INCLUDEPATH += gui
INCLUDEPATH += gui/widgets
INCLUDEPATH += ../qkcore
INCLUDEPATH += ../qkapi/qt/qkapi

LIBS += -lqkcore
QMAKE_LIBDIR += ../qkcore/release

LIBS += -lqkapi
QMAKE_LIBDIR += ../qkapi/qt/qkapi/release

#CONFIG += extserialport

SOURCES += main.cpp\
        gui/mainwindow.cpp \
    core/qkdaemon.cpp \
    gui/qkexplorerwidget.cpp \
    gui/qkdaemonwidget.cpp \
    gui/qkconnectwidget.cpp \
    gui/widgets/ptablewidget.cpp \
    gui/widgets/ptoolbutton.cpp \
    gui/qkrawwidget.cpp \
    gui/widgets/qhexspinbox.cpp \
    gui/widgets/cpropertybrowser.cpp \
    gui/widgets/cproperty.cpp \
    gui/widgets/ptreeitemproperty.cpp \
    gui/settingsdialog.cpp \
    core/qkdaemonthread.cpp \
    core/qkdaemonserver.cpp \
    core/qkdaemonsocket.cpp \
    core/qkapihandler.cpp \
    core/qkconnect.cpp \
    core/qkconnectthread.cpp \
    gui/boardpanel.cpp \
    gui/messagesdialog.cpp \
    gui/eventwidget.cpp \
    gui/widgets/rtplot.cpp \
    gui/widgets/qcustomplot.cpp \
    gui/plotsettings.cpp \
    gui/loggersettingswidget.cpp \
    gui/debugsettingswidget.cpp

HEADERS  += gui/mainwindow.h \
    core/qkdaemon.h \
    gui/qkexplorerwidget.h \
    gui/qkdaemonwidget.h \
    gui/qkconnectwidget.h \
    gui/qkrawwidget.h \
    gui/widgets/ptablewidget.h \
    gui/widgets/ptoolbutton.h \
    gui/widgets/qhexspinbox.h \
    gui/widgets/cpropertybrowser.h \
    gui/widgets/cproperty.h \
    gui/widgets/ptreeitemproperty.h \
    gui/settingsdialog.h \
    core/qkdaemonthread.h \
    core/qkdaemonserver.h \
    core/qkdaemonsocket.h \
    core/qkapihandler.h \
    core/qkconnect.h \
    core/qkconnectthread.h \
    gui/boardpanel.h \
    gui/messagesdialog.h \
    gui/gui_globals.h \
    gui/eventwidget.h \
    gui/widgets/rtplot.h \
    gui/widgets/qcustomplot.h \
    gui/plotsettings.h \
    gui/loggersettingswidget.h \
    gui/debugsettingswidget.h

FORMS    += gui/mainwindow.ui \
    gui/qkexplorerwidget.ui \
    gui/qkdaemonwidget.ui \
    gui/qkconnectwidget.ui \
    gui/qkrawwidget.ui \
    gui/settingsdialog.ui \
    gui/boardpanel.ui \
    gui/messagesdialog.ui \
    gui/eventwidget.ui \
    gui/plotsettings.ui \
    gui/loggersettingswidget.ui \
    gui/debugsettingswidget.ui

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


