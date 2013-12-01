#ifndef QKDAEMON_H
#define QKDAEMON_H

#include <QObject>

#define QKDAEMON_VERSION_MAJOR  0
#define QKDAEMON_VERSION_MINOR  0
#define QKDAEMON_VERSION_PATCH  1

class QkCore;
class QkConnectionManager;
class QTcpServer;
class QkDaemonServer;
class QkAPIHandler;

class QkDaemon : public QObject
{
    Q_OBJECT
public:
    explicit QkDaemon(QkConnectionManager *connManager, QObject *parent = 0);
    ~QkDaemon();

    bool connectToHost(const QString &address, int port);
    void disconnectFromHost();
    bool isListening();

public slots:

signals:
    void statusMessage(QString msg);

private:
    QkDaemonServer *m_server;
    QkConnectionManager *m_connManager;
    QkAPIHandler *m_api;

    QThread *m_thread;
};

#endif // QKDAEMON_H
