#ifndef QKDAEMONSERVER_H
#define QKDAEMONSERVER_H

#include <QTcpServer>
#include <QMap>

class QkDaemonThread;
class QkDaemonSocket;
class QkConnectionManager;

class QkDaemonServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit QkDaemonServer(QkConnectionManager *connManager, QObject *parent = 0);
    
protected:
    void incomingConnection(qintptr socketDescriptor);

signals:
    void info(QString message);
    
public slots:

private slots:
    void _handleClientConnected(int socketDescriptor);
    void _handleClientDisconnected(int socketDescriptor);

private:
    QMap<int, QkDaemonThread*> m_threads;
    QkConnectionManager *m_connManager;
    
};

#endif // QKDAEMONSERVER_H
