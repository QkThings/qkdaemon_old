#include "qkdaemonserver.h"
#include "qkdaemonthread.h"
#include "qkdaemonsocket.h"


QkDaemonServer::QkDaemonServer(QkConnectionManager *connManager, QObject *parent) :
    QTcpServer(parent),
    m_connManager(connManager)
{
}


void QkDaemonServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "newConnection, socketDescriptor =" << socketDescriptor;

    QkDaemonThread *clientThread = new QkDaemonThread(socketDescriptor, m_connManager, this);
    connect(clientThread, SIGNAL(clientConnected(int)),
            this, SLOT(_handleClientConnected(int)));
    connect(clientThread, SIGNAL(clientDisconnected(int)),
            this, SLOT(_handleClientDisconnected(int)), Qt::DirectConnection);
    connect(clientThread, SIGNAL(finished()), clientThread, SLOT(deleteLater()));

    m_threads.insert(socketDescriptor, clientThread);
    clientThread->start();
}

void QkDaemonServer::_handleClientConnected(int socketDescriptor)
{
    QkDaemonSocket *socket = m_threads.value(socketDescriptor)->socket();
    qDebug() << "Client connected: " + socket->peerAddress().toString();
    emit info(tr("Client connected IP: ") + socket->peerAddress().toString());
}

void QkDaemonServer::_handleClientDisconnected(int socketDescriptor)
{
    QkDaemonSocket *socket = m_threads.value(socketDescriptor)->socket();
    qDebug() << "Client disconnected IP: " + socket->peerAddress().toString();
    emit info(tr("Client disconnected IP: ") + socket->peerAddress().toString());
    m_threads.remove(socket->socketDescriptor());
}
