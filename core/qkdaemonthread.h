#ifndef QKDAEMONTHREAD_H
#define QKDAEMONTHREAD_H

#include <QThread>
#include <QTcpSocket>

class QkDaemonThread : public QThread
{
    Q_OBJECT
public:
    explicit QkDaemonThread(int socketDescriptor, QObject *parent = 0);

protected:
    void run();
    
signals:
    void error(QTcpSocket::SocketError socketError);
    
public slots:

private:
    int m_socketDescriptor;
    
};

#endif // QKDAEMONTHREAD_H
