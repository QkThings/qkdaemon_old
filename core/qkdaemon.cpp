#include "qkdaemon.h"

#include "qk.h"

QkDaemon::QkDaemon(QkCore *qk, QObject *parent) :
    QObject(parent),
    m_qk(qk)
{

}

QkDaemon::~QkDaemon()
{

}
