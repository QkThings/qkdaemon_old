#ifndef QKCONNECT_H
#define QKCONNECT_H

#include <QObject>
#include <QtSerialPort/QSerialPort>

class QkCore;

class QkConnect : public QObject
{
    Q_OBJECT
public:
    enum ConnectionType {
        ctSerial,
        ctTCP
    };
    class Connection {
    public:
        ConnectionType type;
        QString param1;
        QString param2;
        QSerialPort *serialPort;
        //QTcpSocket *m_tcp;

        /*bool operator==(Connection *other) {
            if(other->type == type &&
               QString::compare(other->param1, param1) == 0 &&
               QString::compare(other->param2, param2) == 0)
            {
               return true;
            }
            return false;
        }*/
    };

    explicit QkConnect(QkCore *qk, QObject *parent = 0);
    ~QkConnect();

    QSerialPort *getSerialPort();
    
signals:
    void connectionAdded(QkConnect::Connection *c);
    void connectionRemoved(QkConnect::Connection *c);
    
public slots:
    bool validate(ConnectionType type, const QString param1, const QString param2);
    Connection* addConnection(ConnectionType type, const QString param1, const QString param2);
    void removeConnection(ConnectionType type, const QString param1, const QString param2);
    //void slotSerialPortConnect(QString portName, int baudRate);
    //void slorSerialPortDisconnect();
    void slotSerialPortSendBytes(quint8 *buf, int count);
    //void slotSerialPortRead();

private:
    void setupConnections();
    Connection* findConnection(ConnectionType type, const QString param1, const QString param2);

    QkCore *m_qk;
    QList<Connection*>  m_conn;
};

#endif // QKCONNECT_H
