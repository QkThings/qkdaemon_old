#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "qkcore.h"
#include "qkdaemon.h"
#include "qkconnect.h"
#include "qkconnectwidget.h"
#include "qkdaemonwidget.h"
#include "qkexplorerwidget.h"
#include "qkrawwidget.h"

#include "settingsdialog.h"

#include <QDebug>
#include <QToolBar>
#include <QTime>
#include <QSystemTrayIcon>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_connect = new QkConnectionManager();
    m_daemon = new QkDaemon(m_connect);
    m_daemonWidget = new QkDaemonWidget(m_daemon, this);
    m_explorerWidget = new QkExplorerWidget(this);
    m_rawWidget = new QkRawWidget(this);
    m_tools = new QToolBar(tr("Tools"), this);

    m_trayIcon = new QSystemTrayIcon(QIcon(":/img/qk_24.png"));
    //m_trayIconMenu = new QMenu();
    //m_trayIconMenu->addAction("Option 1");
    //m_trayIconMenu->addAction("Option 2");
    //m_trayIcon->setContextMenu(m_trayIconMenu);
    //m_trayIcon->show();

    setupLayout();
    setupConnections();
}

MainWindow::~MainWindow()
{
    delete m_rawWidget;
    delete m_explorerWidget;
    delete m_daemonWidget;
    delete m_connect;
    delete ui;
}

void MainWindow::setupLayout()
{
    ui->menuBar->hide();
    ui->mainToolBar->addWidget(m_daemonWidget);
    ui->mainToolBar->setMovable(false);
    ui->statusBar->hide();
    ui->connectWidget->setQkConnect(m_connect);

    ui->log->hide();

    m_tools->setFloatable(false);
    m_tools->setMovable(false);
    m_tools->setIconSize(QSize(16,16));
    m_tools->addAction(QIcon(":icons/settings_16.png"), "Settings", this, SLOT(slotShowHideSettings()));
    m_tools->addAction(QIcon(":icons/explorer.png"), "QkExplorer", this, SLOT(slotShowHideExplorer()));
    m_tools->addAction(QIcon(":icons/raw.png"), "QkRaw", this, SLOT(slotShowHideRaw()));
    m_tools->addAction(QIcon(":icons/notepad_w_16.png"), "Info", this, SLOT(slotShowHideInfo()));
    addToolBar(Qt::BottomToolBarArea, m_tools);

    setWindowTitle("QkDaemon");
    setWindowIcon(QIcon(":img/qk_64.png"));

    QString info;
    info.append("QkDaemon " + QString().sprintf("%d.%d.%d ", QKDAEMON_VERSION_MAJOR,
                                                QKDAEMON_VERSION_MINOR,
                                                QKDAEMON_VERSION_PATCH));
    info.append("using QkLib " + QkCore::version());
    logMessage(info);
}

void MainWindow::setupConnections()
{
    connect(m_daemon, SIGNAL(statusMessage(QString)), this, SLOT(_handleDaemonStatusMessage(QString)));
    connect(ui->connectWidget, SIGNAL(currentConnectionChanged(QkConnection*)),
            m_rawWidget, SLOT(setCurrentConnection(QkConnection*)));
    connect(ui->connectWidget, SIGNAL(currentConnectionChanged(QkConnection*)),
            m_explorerWidget, SLOT(setCurrentConnection(QkConnection*)));
}

void MainWindow::_handleDaemonStatusMessage(QString message)
{
    logMessage(message, lmtInfo);
}

void MainWindow::logMessage(QString text, LogMessageType lmt)
{
    qDebug() << "logMessage()" << text;
    QColor textColor;
    switch(lmt)
    {
    //case lmtInfo: textColor = Qt::blue; break;
    //case lmtError: textColor = Qt::red; break;
    default:
        textColor = Qt::black;
    }
    ui->log->setTextColor(textColor);

    QString msg;

    if(lmt == lmtInfo || lmt == lmtError)
        msg.append("[" + QTime::currentTime().toString("hh:mm:ss") + "] ");
    msg.append(text);
    ui->log->append(msg);
}

void MainWindow::slotShowHideInfo()
{
    ui->log->setVisible(!ui->log->isVisible());
}

void MainWindow::slotShowHideSettings()
{
    SettingsDialog dialog;
    dialog.exec();
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

