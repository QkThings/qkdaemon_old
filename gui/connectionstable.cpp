#include "connectionstable.h"

#include "qkconnect.h"
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QPushButton>

ConnectionsTable::ConnectionsTable(QWidget *parent) :
    QTableWidget(parent)
{
    setColumnCount(4);
    QHeaderView *header = horizontalHeader();
    header->setSectionResizeMode(ColumnConnect , QHeaderView::ResizeToContents);
    header->setSectionResizeMode(ColumnConnType, QHeaderView::Fixed);
    header->setSectionResizeMode(ColumnParam1  , QHeaderView::Fixed);
    header->setSectionResizeMode(ColumnParam2  , QHeaderView::Stretch);

    setColumnWidth(ColumnConnType, 80);
    setColumnWidth(ColumnParam1, 80);
}

void ConnectionsTable::setQkConnect(QkConnect *conn)
{
    m_conn = conn;
    setupConnections();
}

QSize ConnectionsTable::sizeHint()
{
    return QSize(100,80);
}

void ConnectionsTable::setupConnections()
{
    connect(m_conn, SIGNAL(connectionAdded(QkConnect::Connection*)),
            this, SLOT(slotAddConnection(QkConnect::Connection*)));
}

void ConnectionsTable::slotAddConnection(QkConnect::Connection *conn)
{
    int i,c,r = rowCount();
    insertRow(r);

    QString connTypeName;
    switch(conn->descriptor.type)
    {
    case QkConnect::ctSerial: connTypeName = "Serial Port"; break;
    case QkConnect::ctTCP: connTypeName = "TCP/IP"; break;
    default: connTypeName = "???";
    }

    setItem(r, ColumnConnType, new QTableWidgetItem(connTypeName));
    for(i=0, c = ColumnParam1; i < conn->descriptor.params.count(); c++, i++)
    {
        setItem(r, c, new QTableWidgetItem(conn->descriptor.params.at(i)));
    }
    //setItem(r, ColumnParam1, new QTableWidgetItem(c->param1));
    //setItem(r, ColumnParam2, new QTableWidgetItem(c->param2));
    setCellWidget(r, ColumnConnect, new QPushButton("Open"));
}
