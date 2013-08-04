#ifndef QKCONNECTWIDGET_H
#define QKCONNECTWIDGET_H

#include <QWidget>

namespace Ui {
class QkConnectWidget;
}

#include "qkconnect.h"

class pToolButton;

class QkConnectWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit QkConnectWidget(QWidget *parent = 0);
    ~QkConnectWidget();

    void setQkConnect(QkConnect *conn);

signals:
    void currentConnectionChanged(QkConnection *conn);

public slots:
    void slotAddConnection();
    void slotRemoveConnection();
    void slotConnectionAdded(QkConnection *conn);
    void slotConnectionRemoved(QkConnection *conn);
    void slotReloadAvailableSerialPorts();
    void slotOpenCloseConnection();
    void slotCurrentCellChanged(int curRow, int curCol, int prevRow, int prevCol);
    void slotShowError(QString message);

private slots:
    void updateInterface();
    
private:
    enum {
        ColumnConnType,
        ColumnParam1,
        ColumnParam2,
        ColumnOpenClose,
        ColumnMaxCount
    };

    void setupLayout();
    void setupConnections();
    void fillRow(int row, QkConnection *conn);
    int findConnection(const QkConnection::Descriptor &connDesc);
    QkConnection::Descriptor connectionDescriptor(int row);

    Ui::QkConnectWidget *ui;
    QkConnect *m_connect;
};

#endif // QKCONNECTWIDGET_H
