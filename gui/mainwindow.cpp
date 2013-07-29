#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "qk.h"
#include "qkconnect.h"
#include "qkconnectwidget.h"
#include "qkdaemonwidget.h"
#include "qkexplorerwidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_qk = new QkCore();
    m_connect = new QkConnect(m_qk);
    m_daemonWidget = new QkDaemonWidget(m_connect, this);

    setupLayout();
    setupConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupLayout()
{
    ui->menuBar->hide();
    ui->mainToolBar->addWidget(m_daemonWidget);
    //ui->mainToolBar->addWidget(ui->connectWidget);
    ui->mainToolBar->setMovable(false);
    ui->statusBar->hide();
    ui->connectWidget->setQkConnect(m_connect);

    setWindowTitle("QkDaemon");
    logMessage("Using QkLib " + m_qk->version());
}

void MainWindow::setupConnections()
{

}

void MainWindow::logMessage(QString text)
{
    ui->textEdit->append(text);
}

void MainWindow::slotShowExplorer()
{

}
