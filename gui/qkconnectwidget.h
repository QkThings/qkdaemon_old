#ifndef QKCONNECTWIDGET_H
#define QKCONNECTWIDGET_H

#include <QWidget>

namespace Ui {
class QkConnectWidget;
}

class QkConnect;

class QkConnectWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit QkConnectWidget(QkConnect *conn, QWidget *parent = 0);
    ~QkConnectWidget();

public slots:
    //void setConnectionToRemove(QkConnect::Connection *c);

private slots:
    void slotRefreshSerialPorts();
    void slotAddConnection();
    void slotUpdateInterface();
    
private:
    void setupConnections();
    void setupLayout();

    Ui::QkConnectWidget *ui;

    QkConnect *m_conn;
};

#endif // QKCONNECTWIDGET_H
