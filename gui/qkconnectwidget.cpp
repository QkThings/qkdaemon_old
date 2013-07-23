#include "qkconnectwidget.h"
#include "ui_qkconnectwidget.h"

QkConnectWidget::QkConnectWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QkConnectWidget)
{
    ui->setupUi(this);
}

QkConnectWidget::~QkConnectWidget()
{
    delete ui;
}
