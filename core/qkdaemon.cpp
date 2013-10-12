#include "qkdaemon.h"

#include "qkcore.h"
#include "qkdaemonserver.h"
#include "qkdaemonsocket.h"

#include <QThread>

QkDaemon::QkDaemon(QkConnectionManager *connManager, QObject *parent) :
    QObject(parent),
    m_connManager(connManager)
{

    m_server = new QkDaemonServer(connManager, this);
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
