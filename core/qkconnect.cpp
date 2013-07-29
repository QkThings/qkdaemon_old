#include "qkconnect.h"

#include <QDebug>
#include <QtSerialPort/QSerialPortInfo>

QkConnect::QkConnect(QkCore *qk, QObject *parent) :
    QObject(parent),
    m_qk(qk)
{

}

QkConnect::~QkConnect()
{

}

QList<QkConnect::Connection*> QkConnect::connections()
{
    return m_conn;
}

void QkConnect::setupConnections()
{
    /*connect(m_serialPort, SIGNAL(readyRead()),
            this, SLOT(slotReadyRead());*/
}

void QkConnect::slotSerialPortSendBytes(quint8 *buf, int count)
{

}

bool QkConnect::validate(ConnectionType type, const QString param1, const QString param2)
{
    if(type == ctSerial)
    {
        QString portName = param1;
        //int baudRate = QString::number(param2);
        bool validPort = false;

        foreach(QSerialPortInfo sp, QSerialPortInfo::availablePorts())
        {
            if(sp.portName() == portName)
            {
                validPort = true;
            }
        }
        return validPort;
    }
}

QkConnect::Connection* QkConnect::addConnection(ConnectionType type, const QString param1, const QString param2)
{
    Connection *conn = new Connection();

    if(findConnection(type, param1, param2) != 0)
    {
        emit error("Connection already exists.");
        delete conn;
        return 0;
    }

    conn->type = type;
    conn->param1 = param1;
    conn->param2 = param2;

    if(conn->type == ctSerial)
    {
        conn->serialPort = new QSerialPort(this);
        conn->serialPort->setPortName(param1);
        conn->serialPort->setBaudRate(param2.toInt());
    }
    else if(conn->type == ctTCP)
    {

    }

    m_conn.append(conn);
    emit connectionAdded(conn);

    return conn;
}

void QkConnect::removeConnection(ConnectionType type, const QString param1, const QString param2)
{

}

QkConnect::Connection* QkConnect::findConnection(ConnectionType type, const QString param1, const QString param2)
{
    foreach(Connection *conn, m_conn)
    {
        if(conn->type == type)
        {
            switch(type)
            {
            case ctSerial:
                if(conn->param1 == param1)
                    return conn;
                break;
            case ctTCP:
                if(conn->param1 == param1 && conn->param2 == param2) {
                    return conn;
                }
                break;
            }
        }
    }
    return 0;
}
