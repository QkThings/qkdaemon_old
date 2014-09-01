/*
 * QkThings LICENSE
 * The open source framework and modular platform for smart devices.
 * Copyright (C) 2014 <http://qkthings.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "ui_mainwindow.h"

#include "gui_globals.h"
#include "qkcore.h"
#include "qkdaemon.h"
#include "qkconnect.h"
#include "qkconnectwidget.h"
#include "qkdaemonwidget.h"
#include "qkexplorerwidget.h"
#include "messagesdialog.h"
#include "settingsdialog.h"

#include <QDebug>
#include <QToolBar>
#include <QTime>
#include <QSystemTrayIcon>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_connManager = new QkConnectionManager(this);
    m_connManager->setSearchOnConnect(true);

    m_daemon = new QkDaemon(m_connManager);
    m_daemonWidget = new QkDaemonWidget(m_daemon, this);
    m_explorerWidget = new QkExplorerWidget(this);
    m_tools = new QToolBar(tr("Tools"), this);

    m_messagesDialog = new MessagesDialog(this);
    m_messagesDialog->hide();

    setupLayout();
    setupConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupLayout()
{
    ui->menuBar->hide();
    ui->mainToolBar->addWidget(m_daemonWidget);
    ui->mainToolBar->setMovable(false);
    ui->statusBar->hide();
    ui->connectWidget->setQkConnect(m_connManager);

    ui->log->hide();

    m_tools->setFloatable(false);
    m_tools->setMovable(false);
    m_tools->setIconSize(QSize(16,16));

    m_tools->addAction(QIcon(":icons/settings_16.png"), "Settings", this, SLOT(slotShowHideSettings()))->setEnabled(false);
    m_tools->addAction(QIcon(":icons/raw.png"), "Explorer", this, SLOT(slotShowHideExplorer()));
    /*m_tools->addAction(QIcon(":icons/raw.png"), "Raw", this, SLOT(slotShowHideRaw()));*/
    m_tools->addAction(QIcon(":icons/message_16.png"), "Info", this, SLOT(slotShowHideInfo()));
    addToolBar(Qt::BottomToolBarArea, m_tools);


    setWindowTitle(GUI_MAINWINDOW_TITLE);
    setWindowIcon(QIcon(":img/logo_64.png"));

    QString info;
    info.append("QkDaemon " + QString().sprintf("%d.%d.%d ", QKDAEMON_VERSION_MAJOR,
                                                QKDAEMON_VERSION_MINOR,
                                                QKDAEMON_VERSION_PATCH));
//    info.append("(QkCore " + QKCORE_LIB_VERSION + ")");
    log(info);
}

void MainWindow::setupConnections()
{
    connect(m_daemon, SIGNAL(statusMessage(QString)), this, SLOT(handleDaemonStatusMessage(QString)));

    connect(ui->connectWidget, SIGNAL(currentConnectionChanged(QkConnection*)),
            m_explorerWidget, SLOT(setConnection(QkConnection*)));
}

void MainWindow::handleDaemonStatusMessage(QString message)
{
    log(message, MessagesDialog::mtInfo);
}

void MainWindow::log(const QString &message, MessagesDialog::MessageType type)
{
    qDebug() << "log()" << message;

    m_messagesDialog->log(message, type);
}

void MainWindow::slotShowHideInfo()
{
    //ui->log->setVisible(!ui->log->isVisible());
    m_messagesDialog->setVisible(!m_messagesDialog->isVisible());
}

void MainWindow::slotShowHideSettings()
{
    SettingsDialog dialog;
    dialog.exec();
}

void MainWindow::slotShowHideExplorer()
{
    if(m_explorerWidget->isHidden())
        m_explorerWidget->show();
    else
        m_explorerWidget->hide();
}
