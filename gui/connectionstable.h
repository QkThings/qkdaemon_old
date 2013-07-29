#ifndef CONNECTIONSTABLE_H
#define CONNECTIONSTABLE_H

#include <QTableWidget>

#include <qkconnect.h>

class ConnectionsTable : public QTableWidget
{
    Q_OBJECT
public:
    explicit ConnectionsTable(QWidget *parent = 0);

    void setQkConnect(QkConnect *conn);

    QSize sizeHint();
    
signals:
    
public slots:
    void slotAddConnection(QkConnect::Connection *c);

private:
    enum {
        ColumnConnect,
        ColumnConnType,
        ColumnParam1,
        ColumnParam2
    };

    void setupConnections();
    
    QkConnect *m_conn;
};

#endif // CONNECTIONSTABLE_H
