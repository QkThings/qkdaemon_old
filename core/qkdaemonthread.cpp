#include "qkdaemonthread.h"

#include "qkdaemonclient.h"

QkDaemonThread::QkDaemonThread(int socketDescriptor, QObject *parent) :
    QThread(parent),
    m_socketDescriptor(socketDescriptor)
{
}


void QkDaemonThread::run()
{
    QkDaemonClient client;
    if(!client.setSocketDescriptor(m_socketDescriptor))
    {
        emit error(client.error());
        return;
    }
}
