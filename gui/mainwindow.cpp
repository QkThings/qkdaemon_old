#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "qk.h"
#include "qkconnect.h"
#include "qkconnectwidget.h"
#include "qkdaemonwidget.h"
#include "qkexplorerwidget.h"

#include <QToolBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_qk = new QkCore();
    m_connect = new QkConnect(m_qk);
    m_daemonWidget = new QkDaemonWidget(m_connect, this);
    m_explorerWidget = new QkExplorerWidget(this);
    m_tools = addToolBar(tr("Tools"));
    //m_tools->setMovable(false);
    m_tools->addAction(QIcon(":icons/explorer.png"), "QkExplorer", this, SLOT(slotShowHideExplorer()));

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
    //ui->mainToolBar->setMovable(false);
    //ui->statusBar->hide();
    ui->textEdit->hide();
    ui->connectWidget->setQkConnect(m_connect);

    setWindowTitle("QkDaemon");
    logMessage("Using QkLib " + m_qk->version());
}

void MainWindow::setupConnections()
{
    connect(ui->connectWidget, SIGNAL(currentConnectionChanged(QkConnection*)),
            m_explorerWidget, SLOT(setCurrentConnection(QkConnection*)));
}

void MainWindow::logMessage(QString text)
{
    ui->textEdit->append(text);
}

void MainWindow::slotShowHideExplorer()
{
    if(m_explorerWidget->isHidden())
    {
        m_explorerWidget->show();
    }
    else
    {
        m_explorerWidget->hide();
    }
}
