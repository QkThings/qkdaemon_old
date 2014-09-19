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

#ifndef QKDAEMON_H
#define QKDAEMON_H

#include <QObject>

#define QKDAEMON_VERSION_MAJOR  0
#define QKDAEMON_VERSION_MINOR  0
#define QKDAEMON_VERSION_PATCH  1

class QkCore;
class QkConnectionManager;
class QTcpServer;
class QkDaemonServer;
class QkAPIHandler;

class QkDaemon : public QObject
{
    Q_OBJECT
public:
    explicit QkDaemon(QkConnectionManager *connManager, QObject *parent = 0);
    ~QkDaemon();

    bool connectToHost(const QString &address, int port);
    void disconnectFromHost();
    bool isListening();

public slots:

signals:
    void statusMessage(QString msg);

private:
    QkDaemonServer *m_server;
    QkConnectionManager *m_connManager;
    QkAPIHandler *m_api;

    QThread *m_thread;
};

#endif // QKDAEMON_H
