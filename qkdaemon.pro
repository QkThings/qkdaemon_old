#-------------------------------------------------
#
# Project created by QtCreator 2013-07-22T20:19:20
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport printsupport

TARGET = qkdaemon
TEMPLATE = app

#DEFINES += QT_NO_DEBUG_OUTPUT

INCLUDEPATH += core
INCLUDEPATH += gui
INCLUDEPATH += gui/widgets
INCLUDEPATH += ../utils

INCLUDEPATH += ../qkcore
INCLUDEPATH += ../qkapi/qt
INCLUDEPATH += ../qkwidget

win32 {
QMAKE_LIBDIR += C:\Users\mribeiro\qkthings_local\build\qt\qkcore\release
LIBS += -LC:\Users\mribeiro\qkthings_local\build\qt\qkcore\release -lqkcore

QMAKE_LIBDIR += C:\Users\mribeiro\qkthings_local\build\qt\qkapi\release
LIBS += -LC:\Users\mribeiro\qkthings_local\build\qt\qkapi\release -lqkapi

QMAKE_LIBDIR += C:\Users\mribeiro\qkthings_local\build\qt\qkwidget\release
LIBS += -LC:\Users\mribeiro\qkthings_local\build\qt\qkwidget\release -lqkwidget
}

unix {

QMAKE_LIBDIR += /home/$$(USER)/qkthings_local/build/qt/qkcore/release
LIBS += -L/home/$$(USER)/qkthings_local/build/qt/qkcore/release -lqkcore

QMAKE_LIBDIR += /home/$$(USER)/qkthings_local/build/qt/qkapi/release
LIBS += -L/home/$$(USER)/qkthings_local/build/qt/qkapi/release -lqkapi

QMAKE_LIBDIR += /home/$$(USER)/qkthings_local/build/qt/qkwidget/release
LIBS += -L/home/$$(USER)/qkthings_local/build/qt/qkwidget/release -lqkwidget

QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN/../shared/lib/\',-z,origin'
QMAKE_RPATHDIR += /home/$$(USER)/qkthings_local/build/qt/qkcore/release
QMAKE_RPATHDIR += /home/$$(USER)/qkthings_local/build/qt/qkapi/release
QMAKE_RPATHDIR += /home/$$(USER)/qkthings_local/build/qt/qkwidget/release
}

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
    gui/messagesdialog.cpp \
    ../utils/qkgui.cpp

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
    gui/gui_globals.h \
    ../utils/qkgui.h

FORMS    += gui/mainwindow.ui \
    gui/qkdaemonwidget.ui \
    gui/qkconnectwidget.ui \
    gui/settingsdialog.ui \
    gui/messagesdialog.ui

RESOURCES += \
    resources/api/qkdaemon_api.qrc \
    resources/icons/qkdaemon_icons.qrc \
    resources/img/qkdaemon_img.qrc \
    resources/img.qrc

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

#deploy.commands = python deploy.py

#QMAKE_EXTRA_TARGETS += deploy
#POST_TARGETDEPS += deploy
