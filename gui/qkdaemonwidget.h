#ifndef QKDAEMONWIDGET_H
#define QKDAEMONWIDGET_H

#include <QWidget>

namespace Ui {
class QkDaemonWidget;
}

class QkConnect;

class QkDaemonWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit QkDaemonWidget(QkConnect *conn, QWidget *parent = 0);
    ~QkDaemonWidget();

public slots:
    //void setConnectionToRemove(QkConnect::Connection *c);

private slots:
    void slotRefreshSerialPorts();
    void slotAddConnection();
    void slotUpdateInterface();
    
private:
    void setupConnections();
    void setupLayout();

    Ui::QkDaemonWidget *ui;

    QkConnect *m_conn;
};

#endif // QKDAEMONWIDGET_H
