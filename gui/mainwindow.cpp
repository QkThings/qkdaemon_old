#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "qk.h"
#include "qkconnect.h"
#include "qkconnectwidget.h"
#include "qkdaemonwidget.h"
#include "qkexplorerwidget.h"
#include "qkrawwidget.h"

#include <QDebug>
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
    m_rawWidget = new QkRawWidget(this);
    m_tools = new QToolBar(tr("Tools"), this);
    m_tools->setFloatable(false);
    m_tools->setIconSize(QSize(16,16));
    m_tools->addAction(QIcon(":icons/explorer.png"), "QkExplorer", this, SLOT(slotShowHideExplorer()));
    m_tools->addAction(QIcon(":icons/raw.png"), "QkRaw", this, SLOT(slotShowHideRaw()));
    addToolBar(Qt::BottomToolBarArea, m_tools);

    setupLayout();
    setupConnections();
}

MainWindow::~MainWindow()
{
    delete m_rawWidget;
    delete m_explorerWidget;
    delete m_daemonWidget;
    delete m_connect;
    delete m_qk;
    delete ui;
}

void MainWindow::setupLayout()
{
    ui->menuBar->hide();
    ui->mainToolBar->addWidget(m_daemonWidget);
    //ui->mainToolBar->setMovable(false);
    ui->statusBar->hide();
    ui->connectWidget->setQkConnect(m_connect);

    setWindowTitle("QkDaemon");
    logMessage("Using QkLib " + m_qk->version());
}

void MainWindow::setupConnections()
{
    connect(ui->connectWidget, SIGNAL(currentConnectionChanged(QkConnection*)),
            m_explorerWidget, SLOT(setCurrentConnection(QkConnection*)));
    connect(ui->connectWidget, SIGNAL(currentConnectionChanged(QkConnection*)),
            m_rawWidget, SLOT(setCurrentConnection(QkConnection*)));
}

void MainWindow::logMessage(QString text)
{
    qDebug() << "logMessage()" << text;
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

void MainWindow::slotShowHideRaw()
{
    if(m_rawWidget->isHidden())
    {
        m_rawWidget->show();
    }
    else
    {
        m_rawWidget->hide();
    }
}

