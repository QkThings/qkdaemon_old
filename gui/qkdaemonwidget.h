#ifndef QKDAEMONWIDGET_H
#define QKDAEMONWIDGET_H

#include <QWidget>

namespace Ui {
class QkDaemonWidget;
}

class QkDaemon;
class QkConnect;
class QkExplorerWidget;

class QkDaemonWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit QkDaemonWidget(QkDaemon *daemon, QWidget *parent = 0);
    ~QkDaemonWidget();

public slots:

private slots:
    void _slotConnect();
    void updateInterface();

    
private:
    void setupConnections();
    void setupLayout();

    Ui::QkDaemonWidget *ui;

    QkDaemon *m_daemon;
    QkConnect *m_conn;
    bool m_connected;
};

#endif // QKDAEMONWIDGET_H
