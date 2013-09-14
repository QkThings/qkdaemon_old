#ifndef QKDAEMONCLIENT_H
#define QKDAEMONCLIENT_H

#include <QTcpSocket>

class QkDaemonClient : public QTcpSocket
{
    Q_OBJECT
public:
    explicit QkDaemonClient(QObject *parent = 0);
    
signals:
    
public slots:
    
};

#endif // QKDAEMONCLIENT_H
