#ifndef QKCONNECTTHREAD_H
#define QKCONNECTTHREAD_H

#include <QThread>
class QkConnectionManager;

class QkConnectThread : public QThread
{
    Q_OBJECT
public:
    explicit QkConnectThread(QObject *parent = 0);

    QkConnectionManager* manager();
    
signals:
    
public slots:

private:
    QkConnectionManager *m_connManager;
    
};

#endif // QKCONNECTTHREAD_H
