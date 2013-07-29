#ifndef QKCONNECTWIDGET_H
#define QKCONNECTWIDGET_H

#include <QWidget>

namespace Ui {
class QkConnectWidget;
}

#include "qkconnect.h"

class QkConnectWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit QkConnectWidget(QWidget *parent = 0);
    ~QkConnectWidget();

    void setQkConnect(QkConnect *conn);

public slots:
    void slotAddConnection();
    void slotRemoveConnection();
    void slotConnectionAdded(QkConnect::Connection *c);
    void slotConnectionRemoved(QkConnect::Connection *c);
    void slotReloadAvailableSerialPorts();
    void slotShowError(QString message);

private slots:
    void updateInterface();
    
private:
    enum {
        ColumnConnType,
        ColumnParam1,
        ColumnParam2,
        ColumnOpenClose
    };

    void setupLayout();
    void setupConnections();
    void fillRow(int row, QkConnect::Connection *c);

    Ui::QkConnectWidget *ui;
    QkConnect *m_connect;
};

#endif // QKCONNECTWIDGET_H
