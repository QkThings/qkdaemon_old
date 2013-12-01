#include "qkconnectthread.h"

#include "qkconnect.h"

QkConnectThread::QkConnectThread(QObject *parent) :
    QThread(parent)
{
    m_connManager = new QkConnectionManager(this);
}


QkConnectionManager* QkConnectThread::manager()
{
    return m_connManager;
}
