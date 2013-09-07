#ifndef QKDAEMON_H
#define QKDAEMON_H

#include <QObject>

#define QKDAEMON_VERSION_MAJOR  0
#define QKDAEMON_VERSION_MINOR  0
#define QKDAEMON_VERSION_PATCH  1

class QkCore;
class QkConnect;
class QTcpServer;

class QkDaemon : public QObject
{
    Q_OBJECT
public:
    explicit QkDaemon(QkConnect *conn, QObject *parent = 0);
    ~QkDaemon();

    bool connectToHost(const QString &address, int port);
    void disconnectFromHost();
    bool isListening();


    
signals:
    void statusMessage(QString msg);

public slots:

private:
    QTcpServer *m_tcpServer;
    QkConnect *m_conn;
};

#endif // QKDAEMON_H
