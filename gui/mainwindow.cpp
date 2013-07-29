#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "qk.h"
#include "qkconnect.h"
#include "qkconnectwidget.h"
#include "qkexplorerwidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_qk = new QkCore();
    m_conn = new QkConnect(m_qk);
    m_connectWidget = new QkConnectWidget(m_conn, this);
    m_explorerWidget = new QkExplorerWidget(this);

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
    ui->mainToolBar->addWidget(m_connectWidget);
    ui->mainToolBar->setMovable(false);
    ui->statusBar->hide();
    ui->tableWidget->setQkConnect(m_conn);

    setWindowTitle("QkDaemon");
    logMessage("Using QkLib " + m_qk->version());
}

void MainWindow::setupConnections()
{
    connect(ui->showExplorer_toolButton, SIGNAL(clicked()),
            this, SLOT(slotShowExplorer()));
}

void MainWindow::logMessage(QString text)
{
    ui->textEdit->append(text);
}

void MainWindow::slotShowExplorer()
{
    if(m_explorerWidget->isHidden())
        m_explorerWidget->show();
    else
        m_explorerWidget->hide();
}
