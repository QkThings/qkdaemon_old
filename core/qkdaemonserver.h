#ifndef QKDAEMONSERVER_H
#define QKDAEMONSERVER_H

#include <QTcpServer>
#include <QMap>

class QkDaemonServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit QkDaemonServer(QObject *parent = 0);
    
protected:
    void incomingConnection(qintptr socketDescriptor);

signals:
    
public slots:

private:
    //QMap<int, QTcpSocket> m_incomingConns;
    
};

#endif // QKDAEMONSERVER_H
