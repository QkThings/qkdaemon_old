#include "qkdaemon.h"

#include "qk.h"
#include <QTcpServer>

QkDaemon::QkDaemon(QkConnect *conn, QObject *parent) :
    QObject(parent),
    m_conn(conn)
{
    m_tcpServer = new QTcpServer(this);
}

QkDaemon::~QkDaemon()
{

}

bool QkDaemon::connectToHost(const QString &address, int port)
{
    bool ok;
    QHostAddress hostAddress;
    if(address.toLower() == "localhost")
        hostAddress = QHostAddress::LocalHost;
    else
        hostAddress.setAddress(address);

    ok = m_tcpServer->listen(hostAddress, port);
    if(ok)
    {
        QString msg;
        msg.append(tr("Listening on "));
        msg.append(tr("IP: ") + address + " ");
        msg.append(tr("Port: ") + QString::number(port));
        emit statusMessage(msg);
    }
    else
        emit statusMessage(m_tcpServer->errorString());

    return ok;
}

void QkDaemon::disconnectFromHost()
{
    m_tcpServer->close();
    emit statusMessage("Disconnected");
}

bool QkDaemon::isListening()
{
    return m_tcpServer->isListening();
}
