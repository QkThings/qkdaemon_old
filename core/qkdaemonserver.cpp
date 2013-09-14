#include "qkdaemonserver.h"
#include "qkdaemonthread.h"


QkDaemonServer::QkDaemonServer(QObject *parent) :
    QTcpServer(parent)
{
}


void QkDaemonServer::incomingConnection(qintptr socketDescriptor)
{

}
