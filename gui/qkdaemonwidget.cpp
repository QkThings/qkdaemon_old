#include "qkdaemonwidget.h"
#include "ui_qkdaemonwidget.h"

#include "qkconnect.h"

#include <QDebug>
#include <QMessageBox>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>

QkDaemonWidget::QkDaemonWidget(QkConnect *conn, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QkDaemonWidget),
    m_conn(conn)
{
    ui->setupUi(this);

    setupLayout();
    setupConnections();
}

QkDaemonWidget::~QkDaemonWidget()
{
    delete ui;
}

void QkDaemonWidget::setupConnections()
{

}

void QkDaemonWidget::setupLayout()
{

}

void QkDaemonWidget::slotRefreshSerialPorts()
{

}

void QkDaemonWidget::slotAddConnection()
{

}

void QkDaemonWidget::slotUpdateInterface()
{

}
