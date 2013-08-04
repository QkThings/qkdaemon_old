#ifndef QKCONNECT_H
#define QKCONNECT_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QStringList>

#include "qk.h"

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

    static Type typeFromString(const QString &str);
    static QString typeToString(Type type);

    Descriptor descriptor;
    QIODevice *device;
    QkCore qk;

    void setup();

private slots:
    void _slotDataReady();
    void _slotSendBytes(QByteArray frame);

private:

};

class QkConnect : public QObject
{
    Q_OBJECT
public:
    explicit QkConnect(QObject *parent = 0);
    ~QkConnect();

    QList<QkConnection*> connections();
    QkConnection* findConnection(const QkConnection::Descriptor &connDesc);
    
signals:
    void connectionAdded(QkConnection *c);
    void connectionRemoved(QkConnection *c);
    void error(QString message);
    
public slots:
    bool validate(const QkConnection::Descriptor &connDesc);
    QkConnection* addConnection(const QkConnection::Descriptor &connDesc);
    void removeConnection(const QkConnection::Descriptor &connDesc);

private:
    QList<QkConnection*>  m_connections;
};

#endif // QKCONNECT_H
