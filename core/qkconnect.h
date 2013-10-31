#ifndef QKCONNECT_H
#define QKCONNECT_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QStringList>
#include <QMutex>
#include <QThread>

#include "qkcore.h"

class QSerialPort;

class QkConnection : public QObject
{
    Q_OBJECT
public:
    enum Type
    {
        ctSerial,
        ctTCP
    };
    class Descriptor
    {
    public:
        Type type;
        QStringList params;
    };

    QkConnection(QObject *parent = 0);
    ~QkConnection();

    static Type typeFromString(const QString &str);
    static QString typeToString(Type type);

    Descriptor descriptor;
    QIODevice *device;
    QkCore *qk;

    void setup();
    virtual bool tryOpen() = 0;

    bool operator==(const QkConnection &other);

signals:
    void incomingFrame(QByteArray frame);
    void error(QString message);

public slots:
    virtual void slotSendFrame(QByteArray frame) = 0;

protected slots:
    virtual void _slotDataReady() = 0;

protected:
    QThread *m_commThread;

private:
};

class QkSerialConnection : public QkConnection
{
    Q_OBJECT
public:
    QkSerialConnection(QString portName, int baudRate, QObject *parent = 0);

    bool tryOpen();

public slots:
    void slotSendFrame(QByteArray frame);

protected slots:
    void _slotDataReady();

private:
    class Comm
    {
    public:
        QByteArray frame;
        volatile bool txdata;
        volatile bool rxdata;
        volatile bool frameReady;
        volatile bool seq;
        volatile bool dle;
        volatile bool valid;
        volatile int count;
    };
    void parseIncomingData(quint8 data);
    Comm m_comm;
    QSerialPort *m_sp;
    QString m_portName;
    int m_baudRate;
};

class QkConnectionManager : public QObject
{
    Q_OBJECT
public:
    explicit QkConnectionManager(QObject *parent = 0);
    ~QkConnectionManager();

    QList<QkConnection*> connections();
    QkConnection* defaultConnection();
    QkConnection* connection(const QkConnection::Descriptor &connDesc);
    QkConnection* connection(int connID);
    int connectionID(QkConnection *conn);

    void setSearchOnConnect(bool search);

    QMutex *mutex();
    
signals:
    void connectionAdded(QkConnection *c);
    void connectionRemoved(QkConnection *c);
    void error(QString message);
    
public slots:
    bool validate(const QkConnection::Descriptor &connDesc);
    QkConnection* addConnection(const QkConnection::Descriptor &connDesc);
    void removeConnection(const QkConnection::Descriptor &connDesc);

private:
    QList<QkConnection*> m_connections;
    bool m_searchOnConnect;
    QList<QThread *> m_connThreads;
    QMutex m_mutex;
};

#endif // QKCONNECT_H
