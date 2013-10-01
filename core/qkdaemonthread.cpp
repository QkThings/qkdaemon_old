#include "qkdaemonthread.h"

#include "qkdaemonsocket.h"
#include "qkconnectionmanager.h"
#include "qkapihandler.h"

#include <QJsonDocument>

QkDaemonThread::QkDaemonThread(int socketDescriptor, QkConnectionManager *connManager, QObject *parent) :
    QThread(parent),
    m_socketDescriptor(socketDescriptor)
{
    m_socket = 0;
    m_connManager = connManager;
    m_apiHandler = new QkAPIHandler(connManager, this);
    _setupConnections();
}

void QkDaemonThread::_setupConnections()
{
    connect(m_apiHandler, SIGNAL(sendJson(QJsonDocument)), this, SLOT(sendJson(QJsonDocument)), Qt::DirectConnection);
}

QkDaemonSocket* QkDaemonThread::socket()
{
    return m_socket;
}

void QkDaemonThread::run()
{
    QkDaemonSocket socket;
    m_socket = &socket;

    if(!m_socket->setSocketDescriptor(m_socketDescriptor))
    {
        emit error(m_socket->error());
        return;
    }
    emit clientConnected(m_socketDescriptor);
    connect(m_socket, SIGNAL(parsedJsonString(QString)), this, SLOT(_handleParsedJsonString(QString)), Qt::DirectConnection);
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(_handleDisconnected()), Qt::DirectConnection);
    exec();
}

void QkDaemonThread::sendJson(const QJsonDocument &doc)
{
    qDebug() << "QkDaemonThread::sendJson()";
    QByteArray data = doc.toJson();
    if(m_socket->isOpen() && !data.isEmpty())
        m_socket->write(data);
}

void QkDaemonThread::_handleParsedJsonString(QString str)
{
    qDebug() << "QkDaemonThread::_handleParsedJsonString()" << str;
    m_apiHandler->handleJsonRpc(str);
}

void QkDaemonThread::_handleDisconnected()
{
    disconnect(m_apiHandler, SIGNAL(sendJson(QJsonDocument)), this, SLOT(sendJson(QJsonDocument)));
    m_apiHandler->setRealTime(false);
    emit clientDisconnected(m_socketDescriptor);
    quit();
}

