#include "qkconnect.h"

#include "qk.h"

#include <QDebug>
#include <QtSerialPort/QSerialPortInfo>
#include <QStringList>
#include <QIODevice>

QkSerialConnection::QkSerialConnection(QString portName, int baudRate, QObject *parent) :
    QkConnection(parent)
{
    m_sp = new QSerialPort(this);
    device = m_sp;

    m_portName = portName;
    m_baudRate = baudRate;

    m_comm.rxdata = false;
    m_comm.valid = false;
    m_comm.dle = false;
    m_comm.frameReady = false;
}

bool QkSerialConnection::tryOpen()
{

    m_sp->setPortName(m_portName);
    if(m_sp->open(QIODevice::ReadWrite))
    {
        m_sp->setBaudRate(m_baudRate);
        m_sp->setParity(QSerialPort::NoParity);
        m_sp->setFlowControl(QSerialPort::NoFlowControl);
        //m_sp->setFlowControl(QSerialPort::HardwareControl);
        m_sp->setDataBits(QSerialPort::Data8);
        //m_sp->close();
        //m_sp->readAll();
        return true;
    }
    else
    {
        QString errMsg;
        errMsg = tr("Unable to open port ") + m_sp->portName() + ".\n";
        errMsg += m_sp->errorString();
        emit error(errMsg);
        return false;
    }
}

void QkSerialConnection::slotSendFrame(QByteArray frame)
{
    int i;
    // Byte stuffing
    char flagByte = QK_COMM_FLAG;
    char dleByte = QK_COMM_DLE;
    quint8 *frameBuf = (quint8*)frame.data();

    m_sp->write(&flagByte, 1);
    for(i = 0; i < frame.count(); i++)
    {
        if(*frameBuf == QK_COMM_FLAG || *frameBuf == QK_COMM_DLE)
        {
            m_sp->write(&dleByte, 1);
        }
        m_sp->write((char*)frameBuf, 1);
        frameBuf++;
    }
    m_sp->write(&flagByte, 1);
}

void QkSerialConnection::slotDataReady()
{
    //QDebug debug(QtDebugMsg);
    int count;

    // Unstuff bytes
    QByteArray data = device->readAll();
    char *bufPtr = data.data();
    count = data.count();
    //debug << "rx: ";
    while(count--)
    {
        //debug << QString().sprintf("%02X", (quint8)*bufPtr);
        parseIncomingData((quint8)*bufPtr++);
        if(m_comm.frameReady)
        {
            qk.comm_processFrame(m_comm.frame);
            emit incomingFrame(m_comm.frame); //FIXME just emit the signal (?)
            m_comm.frameReady = false;
        }
    }
}

void QkSerialConnection::parseIncomingData(quint8 data)
{
    switch(data)
    {
    case QK_COMM_FLAG:
        if(!m_comm.dle)
        {
            if(!m_comm.rxdata)
            {
                m_comm.frame.clear();
                m_comm.rxdata = true;
                m_comm.valid = true;
            }
            else
            {
                if(m_comm.frame.count() > 0 && m_comm.valid)
                {
                    m_comm.frameReady = true;
                    m_comm.rxdata = false;
                    m_comm.valid = false;
                }
            }
            return;
        }
        break;
    case QK_COMM_DLE:
        if(m_comm.valid)
        {
            if(!m_comm.dle)
            {
                m_comm.dle = true;
                return;
            }
        }
        break;
    default: ;
    }

    if(m_comm.valid)
    {
        m_comm.frame.append(data);
    }

    m_comm.dle = false;
}

QkConnection::QkConnection(QObject *parent) :
    QObject(parent)
{

}

QkConnection::~QkConnection()
{
    qDebug() << "Deleting connection...";
    if(device != 0)
    {
        if(device->isOpen())
            device->close();
        delete device;
    }
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
    return ctSerial; // Must not reach this line
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
            this, SLOT(slotDataReady()));
    connect(&qk, SIGNAL(comm_sendFrame(QByteArray)),
            this, SLOT(slotSendFrame(QByteArray)));
}

QkConnect::QkConnect(QObject *parent) :
    QObject(parent)
{

}

QkConnect::~QkConnect()
{
    qDebug() << "Delete connections:";
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
    qDebug() << "addConnection()";
    QkConnection *conn;

    if(findConnection(connDesc) != 0)
    {
        emit error(tr("Connection already exists."));
        delete conn;
        return 0;
    }

    if(connDesc.type == QkConnection::ctSerial)
    {
        QString portName = connDesc.params.at(0);
        int baudRate = connDesc.params.at(1).toInt();
        qDebug() << portName << baudRate;
        conn = new QkSerialConnection(portName, baudRate, this);
        connect(conn, SIGNAL(error(QString)), this, SIGNAL(error(QString)));
    }
    else if(connDesc.type == QkConnection::ctTCP)
    {

    }
    else
    {
        return 0;
    }

    if(!conn->tryOpen())
    {
        delete conn;
        return 0;
    }

    conn->setup();
    conn->descriptor.type = connDesc.type;
    conn->descriptor.params.clear();
    conn->descriptor.params.append(connDesc.params);

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
