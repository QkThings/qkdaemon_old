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

#include "qkdaemonwidget.h"
#include "ui_qkdaemonwidget.h"

#include "qkdaemon.h"
#include "qkconnect.h"
#include "qkexplorerwidget.h"

#include <QDebug>
#include <QMessageBox>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>

QkDaemonWidget::QkDaemonWidget(QkDaemon *daemon, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QkDaemonWidget),
    m_daemon(daemon)
{
    ui->setupUi(this);

    setupLayout();
    setupConnections();
}

QkDaemonWidget::~QkDaemonWidget()
{
    delete ui;
}

void QkDaemonWidget::setupConnections()
{
    connect(ui->connect_button, SIGNAL(clicked()), this, SLOT(slotConnect()));
}

void QkDaemonWidget::setupLayout()
{

}

void QkDaemonWidget::slotConnect()
{
    if(!m_daemon->isListening())
        m_daemon->connectToHost(ui->hostAddress_lineEdit->text(), ui->port_spinBox->value());
    else
        m_daemon->disconnectFromHost();
    updateInterface();
}

void QkDaemonWidget::updateInterface()
{
    bool canConnect;
    if(m_daemon->isListening())
        canConnect = false;
    else
        canConnect = true;

    if(canConnect)
        ui->connect_button->setText(tr("Connect"));
    else
        ui->connect_button->setText(tr("Disconnect"));

    ui->hostAddress_lineEdit->setEnabled(canConnect);
    ui->port_spinBox->setEnabled(canConnect);
}

