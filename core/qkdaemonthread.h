#ifndef QKDAEMONTHREAD_H
#define QKDAEMONTHREAD_H

#include <QThread>
#include <QTcpSocket>

class QkConnectionManager;
class QkDaemonSocket;
class QkAPIHandler;
class QkConnection;

class QkDaemonThread : public QThread
{
    Q_OBJECT
public:
    explicit QkDaemonThread(int socketDescriptor, QkConnectionManager *connManager, QObject *parent = 0);

    QkDaemonSocket* socket();

protected:
    void run();
    
signals:
    void error(QTcpSocket::SocketError socketError);
    void clientConnected(int socketDescriptor);
    void clientDisconnected(int socketDescriptor);
    
public slots:
    void sendJson(const QJsonDocument &doc);

private slots:
    void _handleParsedJsonString(QString str);
    void _handleDisconnected();

private:
    void _setupConnections();

    int m_socketDescriptor;
    QkDaemonSocket *m_socket;
    QkConnectionManager *m_connManager;
    QkAPIHandler *m_apiHandler;
    
};

#endif // QKDAEMONTHREAD_H
