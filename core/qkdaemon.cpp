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

#include "qkcore.h"
#include "qkdaemonserver.h"
#include "qkdaemonsocket.h"

#include <QThread>

QkDaemon::QkDaemon(QkConnectionManager *connManager, QObject *parent) :
    QObject(parent),
    m_connManager(connManager)
{

    m_server = new QkDaemonServer(connManager, this);

    connect(m_server, SIGNAL(info(QString)), this, SIGNAL(statusMessage(QString)));
}

QkDaemon::~QkDaemon()
{
    delete m_server;
}

bool QkDaemon::connectToHost(const QString &address, int port)
{
    bool ok;
    QHostAddress hostAddress;
    if(address.toLower() == "localhost")
        hostAddress = QHostAddress::LocalHost;
    else
        hostAddress.setAddress(address);

    ok = m_server->listen(hostAddress, port);
    if(ok)
    {
        QString msg;
        msg.append(tr("Listening on "));
        msg.append(tr("IP: ") + address + " ");
        msg.append(tr("Port: ") + QString::number(port));
        emit statusMessage(msg);
    }
    else
        emit statusMessage(m_server->errorString());

    return ok;
}

void QkDaemon::disconnectFromHost()
{
    m_server->close();
    emit statusMessage("Disconnected");
}

bool QkDaemon::isListening()
{
    return m_server->isListening();
}
