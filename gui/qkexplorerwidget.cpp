#include "qkexplorerwidget.h"
#include "ui_qkexplorerwidget.h"

#include "qkcore.h"
#include "cproperty.h"
#include "cpropertybrowser.h"
#include "eventwidget.h"

#include <QDebug>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>
#include <QListWidget>
#include <QListWidgetItem>
#include <QHeaderView>
#include <QMessageBox>
#include <QTime>
#include <QScrollBar>

QkExplorerWidget::QkExplorerWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QkExplorerWidget),
    m_conn(0)
{
    m_debugPrintTime = false;
    m_debugPrintSource = false;

    _setupLayout();
    _setupConnections();
    updateInterface();
}

QkExplorerWidget::~QkExplorerWidget()
{
    delete ui;
}

void QkExplorerWidget::_setupLayout()
{
    ui->setupUi(this);
    ui->menubar->hide();
    ui->statusBar->hide();

//    QListWidgetItem *item = new QListWidgetItem();
//    EventWidget *eventWidget = new EventWidget();
//    ui->eventList->addItem(item);
//    ui->eventList->setItemWidget(item, eventWidget);
//    QHeaderView *header;

//    header = ui->logEventTable->horizontalHeader();
//    header->setSectionResizeMode(LoggerColumnEventTimestamp, QHeaderView::Fixed);
//    header->setSectionResizeMode(LoggerColumnEventSource, QHeaderView::Fixed);
//    header->setSectionResizeMode(LoggerColumnEventLabel, QHeaderView::Fixed);
//    header->setSectionResizeMode(LoggerColumnEventMessage, QHeaderView::Interactive);
//    header->setSectionResizeMode(LoggerColumnEventArguments, QHeaderView::Stretch);

//    ui->logEventTable->setColumnWidth(LoggerColumnEventTimestamp, 70);
//    ui->logEventTable->setColumnWidth(LoggerColumnEventSource, 65);
//    ui->logEventTable->setColumnWidth(LoggerColumnEventLabel, 80);
//    ui->logEventTable->setColumnWidth(LoggerColumnEventMessage, 250);

//    ui->logEventTable->setFrameStyle(QFrame::StyledPanel);
//    ui->logEventTable->setSortingEnabled(false);

#ifdef Q_OS_WIN
    ui->debugText->setFont(QFont("Consolas",9));
#else
    ui->debugText->setFont(QFont("Monospace",9));
#endif

    setWindowTitle("QkExplorer");
    updateInterface();
}

void QkExplorerWidget::_setupConnections()
{
    connect(ui->search_button, SIGNAL(clicked()),
            this, SLOT(_slotSearch()));
    connect(ui->start_button, SIGNAL(clicked()),
            this, SLOT(_slotStart()));
    connect(ui->stop_button, SIGNAL(clicked()),
            this, SLOT(_slotStop()));
    connect(ui->button_clearLogger, SIGNAL(clicked()),
            ui->eventTable, SLOT(removeAll()));
    connect(ui->check_enableLogger, SIGNAL(clicked(bool)),
            this, SLOT(_slotLogger_setEnabled(bool)));
    connect(ui->button_clearDebug, SIGNAL(clicked()),
            ui->debugText, SLOT(clear()));
    connect(ui->check_enableDebug, SIGNAL(clicked(bool)),
            this, SLOT(_slotDebug_setEnabled(bool)));

    connect(ui->explorerList, SIGNAL(currentRowChanged(int)),
            this, SLOT(_handleExplorerListRowChanged(int)));
    connect(ui->explorerList, SIGNAL(currentRowChanged(int)),
            this, SLOT(_slotBoardPanels_reload()));

    connect(ui->check_enableLogger, SIGNAL(clicked()),
            this, SLOT(updateInterface()));
    connect(ui->check_enableDebug, SIGNAL(clicked()),
            this, SLOT(updateInterface()));

    connect(ui->debugPrintTime_check, SIGNAL(clicked()),
            this, SLOT(_slotDebug_updateOptions()));
    connect(ui->debugPrintSource_check, SIGNAL(clicked()),
            this, SLOT(_slotDebug_updateOptions()));

    connect(ui->explorerTabs, SIGNAL(currentChanged(int)),
            this, SLOT(updateInterface()));
}

void QkExplorerWidget::setCurrentConnection(QkConnection *conn)
{
    if(m_conn != 0)
    {
        disconnect(m_conn->qk, SIGNAL(error(int,int)), this, SLOT(showError(int,int)));
        disconnect(m_conn->qk, SIGNAL(deviceFound(int)), this, SLOT(_slotExplorerList_addNode(int)));
        disconnect(m_conn->qk, SIGNAL(deviceUpdated(int)), this, SLOT(_handleNodeUpdated(int)));
        disconnect(m_conn->qk, SIGNAL(dataReceived(int)), this, SLOT(_handleDataReceived(int)));
        disconnect(m_conn->qk, SIGNAL(eventReceived(int,QkDevice::Event)), this, SLOT(_slotLogger_append(int,QkDevice::Event)));
        disconnect(m_conn->qk, SIGNAL(debugString(int,QString)), this, SLOT(_slotDebug_log(int,QString)));
    }

    m_conn = conn;
    connect(m_conn->qk, SIGNAL(error(int,int)), this, SLOT(showError(int,int)));
    connect(m_conn->qk, SIGNAL(deviceFound(int)), this, SLOT(_slotExplorerList_addNode(int)));
    connect(m_conn->qk, SIGNAL(deviceUpdated(int)), this, SLOT(_handleNodeUpdated(int)));
    connect(m_conn->qk, SIGNAL(dataReceived(int)), this, SLOT(_handleDataReceived(int)));
    connect(m_conn->qk, SIGNAL(eventReceived(int,QkDevice::Event)), this, SLOT(_slotLogger_append(int,QkDevice::Event)));
    connect(m_conn->qk, SIGNAL(debugString(int,QString)), this, SLOT(_slotDebug_log(int,QString)));

    updateInterface();
}

void QkExplorerWidget::_handleExplorerListRowChanged(int row)
{
    if(row < 0)
    {
        m_selNode = 0;
        ui->stackedPanels->setCurrentIndex(spiNone);
        return;
    }
    QString itemText = ui->explorerList->item(row)->text();
    if(itemText.contains(tr("Node")))
    {
        m_selBoardType = sbtModuleDevice;
        bool ok;
        QString addrStr = itemText.split(' ').at(1);
        int addr = addrStr.toInt(&ok, 16);
        m_selNode = m_conn->qk->node(addr);
        ui->stackedPanels->setCurrentIndex(spiNode);
    }
    else
    {
        m_selBoardType = sbtGatewayNetwork;
        m_selNode = 0;
    }
    updateInterface();
}


void QkExplorerWidget::_handleDataReceived(int address)
{
    if(m_selNode != 0 && m_selNode->address() == address)
    {
        //_boardPanel_device_refreshData(m_selNode->device());
        ui->deviceBoardPanel->refreshData();
    }
}

void QkExplorerWidget::_handleNodeUpdated(int address)
{
    if(m_selBoardType == sbtModuleDevice)
    {
        if(m_selNode != 0 && m_selNode->address() == address)
            _slotBoardPanels_reload();
    }
}

void QkExplorerWidget::_slotExplorerList_reload() //FIXME is it really needed?
{
    ui->explorerList->clear();
    QList<int> addressList = m_conn->qk->nodes().keys();
    foreach(int address, addressList)
    {
        _slotExplorerList_addNode(address);
    }
}

void QkExplorerWidget::_slotExplorerList_addNode(int address)
{
    if(_explorerList_findNode(address) >= 0)
        return;

    ui->explorerList->addItem(tr("Node") + QString().sprintf(" %04X", address));
    if(ui->explorerList->count() == 1)
    {
        ui->explorerList->setCurrentRow(0);
    }
}

void QkExplorerWidget::_slotBoardPanels_reload()
{    
    if(ui->explorerList->currentRow() < 0)
        return;

    if(m_selBoardType == sbtModuleDevice)
    {
        if(m_selNode == 0)
            return;

        ui->commBoardPanel->setBoard(m_selNode->module(), QkBoard::btModule, m_conn);
        ui->commBoardPanel->reload();
        ui->commBoardPanel->refresh();

        ui->deviceBoardPanel->setBoard(m_selNode->device(), QkBoard::btDevice, m_conn);
        ui->deviceBoardPanel->reload();
        ui->deviceBoardPanel->refresh();

        if(m_selNode->device() != 0)
            ui->nodeTabWidget->setCurrentIndex(1);
        else
            ui->nodeTabWidget->setCurrentIndex(0);
    }
}


int QkExplorerWidget::_explorerList_findNode(int address)
{
    int row;

    for(row = 0; row < ui->explorerList->count(); row++)
    {
        QString itemText = ui->explorerList->item(row)->text();
        if(itemText.contains(tr("Node")))
        {
            bool ok;
            QString addrStr = itemText.split(' ').at(1);
            int addr = addrStr.toInt(&ok, 16);
            if(addr == address)
                return row;
        }
    }
    return -1;
}

void QkExplorerWidget::_slotSearch()
{
    int stackIdx = ui->stackedPanels->currentIndex();
    ui->explorerList->clear();
    ui->stackedPanels->setCurrentIndex(stackIdx);
    m_conn->qk->search();
}

void QkExplorerWidget::_slotStart()
{
    m_conn->qk->start();
}

void QkExplorerWidget::_slotStop()
{
    m_conn->qk->stop();
}



void QkExplorerWidget::_slotClear()
{
    //ui->logEventTable->removeAll();
    ui->debugText->clear();
}

QString QkExplorerWidget::insertArgsOnMessage(QString msg, QList<float> args)
{
    for(int i=0; i<args.count(); i++)
        msg.replace(QRegularExpression("\\%" + QString::number(i)), QString::number(args[i]));

    return msg;
}

void QkExplorerWidget::updateInterface()
{
    bool enableButtons = false;
    if(m_conn == 0)
        enableButtons = false;
    else
        enableButtons = true;

    ui->search_button->setEnabled(enableButtons);
    ui->start_button->setEnabled(enableButtons);
    ui->stop_button->setEnabled(enableButtons);


    bool nothingToShow;
    if(ui->explorerList->count() == 0)
        nothingToShow = true;
    else
        nothingToShow = false;

    ui->label->setHidden(nothingToShow);
    ui->explorerList->setHidden(nothingToShow);

    /*bool logEnabled = ui->check_enableLogger->isChecked();
    ui->logEventTable->setEnabled(logEnabled);

    bool debugEnabled = ui->check_enableDebug->isChecked();
    ui->debugText->setEnabled(debugEnabled);*/
}

void QkExplorerWidget::_slotDebug_log(int address, QString debugStr)
{
    if(!ui->check_enableDebug->isChecked())
        return;

    QString str;
    if(m_debugPrintTime)
        str.append(QTime::currentTime().toString("hh:mm:ss") + " ");
    if(m_debugPrintSource)
        str.append("[" + QString().sprintf("%04X", address) + "] ");
    str.append(debugStr);
    ui->debugText->append(str);
}

void QkExplorerWidget::_slotDebug_updateOptions()
{
    m_debugPrintTime = ui->debugPrintTime_check->isChecked();
    m_debugPrintSource = ui->debugPrintSource_check->isChecked();
}

void QkExplorerWidget::_slotDebug_setEnabled(bool enabled)
{
    if(enabled)
        ui->label_debugEnabled->setPixmap(QPixmap(":/icons/on_16.png"));
    else
        ui->label_debugEnabled->setPixmap(QPixmap(":/icons/off_16.png"));
//    ui->debugText->setEnabled(enabled);
}

void QkExplorerWidget::_slotLogger_append(int address, QkDevice::Event event)
{
    if(!ui->check_enableLogger->isChecked())
        return;

    int r = ui->eventTable->addRow();

    QTableWidgetItem *item = new QTableWidgetItem();
    EventWidget *eventWidget = new EventWidget();
    eventWidget->setEvent(&event, address);
    ui->eventTable->setItem(r, 0, item);
    ui->eventTable->setCellWidget(r, 0, eventWidget);

    ui->eventTable->scrollToBottom();
}

void QkExplorerWidget::_slotLogger_setEnabled(bool enabled)
{
    if(enabled)
        ui->label_loggerEnabled->setPixmap(QPixmap(":/icons/on_16.png"));
    else
        ui->label_loggerEnabled->setPixmap(QPixmap(":/icons/off_16.png"));
//    ui->eventTable->setEnabled(enabled);
}

void QkExplorerWidget::showError(int code, int arg)
{
    showError(QkCore::errorMessage(code) + " " + QString().sprintf("(%d)",arg));
}

void QkExplorerWidget::showError(const QString &message)
{
    QMessageBox::critical(this, tr("Error"), message);
    qDebug() << "ERROR:" << message;
}
