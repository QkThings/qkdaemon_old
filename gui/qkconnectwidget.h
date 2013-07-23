#ifndef QKCONNECTWIDGET_H
#define QKCONNECTWIDGET_H

#include <QWidget>

namespace Ui {
class QkConnectWidget;
}

class QkConnectWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit QkConnectWidget(QWidget *parent = 0);
    ~QkConnectWidget();
    
private:
    Ui::QkConnectWidget *ui;
};

#endif // QKCONNECTWIDGET_H
