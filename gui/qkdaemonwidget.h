#ifndef QKDAEMONWIDGET_H
#define QKDAEMONWIDGET_H

#include <QWidget>

namespace Ui {
class QkDaemonWidget;
}

class QkConnect;
class QkExplorerWidget;

class QkDaemonWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit QkDaemonWidget(QkConnect *conn, QWidget *parent = 0);
    ~QkDaemonWidget();

public slots:

private slots:

    
private:
    void setupConnections();
    void setupLayout();

    Ui::QkDaemonWidget *ui;

    QkConnect *m_conn;
};

#endif // QKDAEMONWIDGET_H
