#ifndef QKDAEMONSOCKET_H
#define QKDAEMONSOCKET_H

//#include <QTcpSocket>
#include "qkapisocket.h"

class QkDaemonSocket : public QkAPISocket
{
    Q_OBJECT
public:
    explicit QkDaemonSocket(QObject *parent = 0);
    
signals:
    
private:

    
};

#endif // QKDAEMONSOCKET_H
