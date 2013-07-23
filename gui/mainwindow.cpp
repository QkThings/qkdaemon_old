#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "qkconnectwidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->menuBar->hide();

    m_connectWidget = new QkConnectWidget(this);
    ui->mainToolBar->addWidget(m_connectWidget);

    setWindowTitle("QkDaemon");
}

MainWindow::~MainWindow()
{
    delete ui;
}
