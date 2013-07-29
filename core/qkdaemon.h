#ifndef QKDAEMON_H
#define QKDAEMON_H

#include <QObject>

#define QKDAEMON_VERSION_MAJOR  0
#define QKDAEMON_VERSION_MINOR  0
#define QKDAEMON_VERSION_PATCH  1

class QkCore;

class QkDaemon : public QObject
{
    Q_OBJECT
public:
    explicit QkDaemon(QkCore *qk, QObject *parent = 0);
    ~QkDaemon();
    
signals:
    
public slots:

private:
    QkCore *m_qk;
};

#endif // QKDAEMON_H
