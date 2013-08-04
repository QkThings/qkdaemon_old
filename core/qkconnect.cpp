#include "qkconnect.h"

#include "qk.h"

#include <QDebug>
#include <QtSerialPort/QSerialPortInfo>
#include <QStringList>
#include <QIODevice>

void QkConnection::_slotDataReady()
{
    qDebug() << "_slotReadyRead";
    qk.comm_processBytes(device->readAll());
}

void QkConnection::_slotSendBytes(QByteArray frame)
{
    qDebug() << "_slotSendBytes" << frame;
    device->write(frame);
}

QkConnection::Type QkConnection::typeFromString(const QString &str)
{
    if(str == "Serial Port")
    {
        return ctSerial;
    }
    else if(str == "TCP/IP")
    {
        return ctTCP;
    }

}

QString QkConnection::typeToString(QkConnection::Type type)
{
    QString str;
    switch(type)
    {
    case QkConnection::ctSerial: str = "Serial Port"; break;
    case QkConnection::ctTCP: str = "TCP/IP"; break;
    default: str = "???";
    }
    return str;
}

void QkConnection::setup()
{
    connect(device, SIGNAL(readyRead()),
            this, SLOT(_slotDataReady()));
    connect(&qk, SIGNAL(comm_sendBytes(QByteArray)),
            this, SLOT(_slotSendBytes(QByteArray)));
}

QkConnect::QkConnect(QObject *parent) :
    QObject(parent)
{

}

QkConnect::~QkConnect()
{

}

QList<QkConnection*> QkConnect::connections()
{
    return m_connections;
}

bool QkConnect::validate(const QkConnection::Descriptor &connDesc)
{
    if(connDesc.type == QkConnection::ctSerial)
    {
        if(connDesc.params.count() != 2)
        {
            return false;
        }
        QString portName = connDesc.params.at(0);;
        bool validPort = false;
        bool validBaud = connDesc.params.at(1).toInt() > 0 ? true : false;

        foreach(QSerialPortInfo sp, QSerialPortInfo::availablePorts())
        {
            if(sp.portName() == portName)
            {
                validPort = true;
            }
        }
        return validPort & validBaud;
    }
    return false;
}

QkConnection* QkConnect::addConnection(const QkConnection::Descriptor &connDesc)
{
    QkConnection *conn = new QkConnection();
    QSerialPort *sp;

    if(findConnection(connDesc) != 0)
    {
        emit error(tr("Connection already exists."));
        delete conn;
        return 0;
    }

    conn->descriptor.type = connDesc.type;
    conn->descriptor.params.clear();
    conn->descriptor.params << connDesc.params;

    if(connDesc.type == QkConnection::ctSerial)
    {
        sp = new QSerialPort(this);
        sp->setPortName(connDesc.params.at(0));
        sp->setBaudRate(connDesc.params.at(1).toInt());
        sp->setParity(QSerialPort::NoParity);
        sp->setFlowControl(QSerialPort::NoFlowControl);
        sp->setDataBits(QSerialPort::Data8);
        if(sp->open(QIODevice::ReadWrite))
        {
            conn->device = sp;
        }
        else
        {
            error(tr("Unable to open port") + sp->portName());
            delete conn;
            return 0;
        }
    }
    else if(connDesc.type == QkConnection::ctTCP)
    {

    }

    conn->setup();

    m_connections.append(conn);
    emit connectionAdded(conn);

    return conn;
}

void QkConnect::removeConnection(const QkConnection::Descriptor &connDesc)
{
    QkConnection *conn = findConnection(connDesc);
    if(conn != 0)
    {
        if(conn->device->isOpen())
        {
            conn->device->close();
        }
        emit connectionRemoved(conn);
        m_connections.removeOne(conn);
        delete conn;
    }
}

QkConnection* QkConnect::findConnection(const QkConnection::Descriptor &connDesc)
{
    foreach(QkConnection *conn, m_connections)
    {
        if(conn->descriptor.type == connDesc.type)
        {
            switch(connDesc.type)
            {
            case QkConnection::ctSerial:
                if(conn->descriptor.params.at(0) == connDesc.params.at(0))
                    return conn;
                break;
            case QkConnection::ctTCP:
                break;
            }
        }
    }
    return 0;
}
