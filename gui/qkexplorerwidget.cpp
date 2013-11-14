#include "qkexplorerwidget.h"
#include "ui_qkexplorerwidget.h"

#include "qkcore.h"
#include "cproperty.h"
#include "cpropertybrowser.h"

#include <QDebug>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>
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

    setupLayout();
    setupConnections();
    updateInterface();
}

QkExplorerWidget::~QkExplorerWidget()
{
    delete ui;
}

void QkExplorerWidget::setupLayout()
{
    ui->setupUi(this);
    ui->menubar->hide();
    ui->statusBar->hide();

    QHeaderView *header;

    /*header = ui->explorerTreeLeft->header();
    header->setSectionResizeMode(ExplorerTreeColumnElement, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(ExplorerTreeColumnValue, QHeaderView::Stretch);
    header->setSectionResizeMode(ExplorerTreeColumnPopup, QHeaderView::Fixed);*/

    header = ui->explorerTreeRight->header();
    header->setSectionResizeMode(ExplorerTreeColumnElement, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(ExplorerTreeColumnValue, QHeaderView::Stretch);
    header->setSectionResizeMode(ExplorerTreeColumnPopup, QHeaderView::Fixed);

    //ui->explorerTreeLeft->setColumnWidth(ExplorerTreeColumnPopup, 16);
    ui->explorerTreeRight->setColumnWidth(ExplorerTreeColumnPopup, 16);

    //ui->explorerTreeLeft->setColumnHidden(ExplorerTreeColumnPopup, true);
    ui->explorerTreeRight->setColumnHidden(ExplorerTreeColumnPopup, true);

    //ui->explorerTreeLeft->setIndentation(10);
    ui->explorerTreeRight->setIndentation(10);

    //ui->boardTypeLabelLeft->hide();
    //ui->boardTypeLabelRight->hide();
    //ui->explorerTreeLeft->hide();

    header = ui->logEventTable->horizontalHeader();
    header->setSectionResizeMode(LoggerColumnEventTimestamp, QHeaderView::Fixed);
    header->setSectionResizeMode(LoggerColumnEventSource, QHeaderView::Fixed);
    header->setSectionResizeMode(LoggerColumnEventMessage, QHeaderView::Interactive);
    header->setSectionResizeMode(LoggerColumnEventArguments, QHeaderView::Stretch);

    ui->logEventTable->setColumnWidth(LoggerColumnEventTimestamp, 70);
    ui->logEventTable->setColumnWidth(LoggerColumnEventSource, 70);
    ui->logEventTable->setColumnWidth(LoggerColumnEventMessage, 300);

    ui->logEventTable->setFrameStyle(QFrame::StyledPanel);
    ui->logEventTable->setSortingEnabled(false);

#ifdef Q_OS_WIN
    ui->logEventTable->setFont(QFont("Consolas", 9));
    ui->debugText->setFont(QFont("Consolas",9));
#endif

    setWindowTitle("QkExplorer");
    updateInterface();
}

void QkExplorerWidget::setupConnections()
{
    connect(ui->search_button, SIGNAL(clicked()),
            this, SLOT(_slotSearch()));
    connect(ui->start_button, SIGNAL(clicked()),
            this, SLOT(_slotStart()));
    connect(ui->stop_button, SIGNAL(clicked()),
            this, SLOT(_slotStop()));
    connect(ui->update_button, SIGNAL(clicked()),
            this, SLOT(_slotUpdate()));
    connect(ui->clear_button, SIGNAL(clicked()),
            this, SLOT(_slotClear()));

    connect(ui->explorerList, SIGNAL(currentRowChanged(int)),
            this, SLOT(_handleExplorerListRowChanged(int)));
    connect(ui->explorerList, SIGNAL(currentRowChanged(int)),
            this, SLOT(_slotExplorerTrees_reload()));

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
    }
    else
    {
        m_selBoardType = sbtGatewayNetwork;
        m_selNode = 0;
    }
}

void QkExplorerWidget::_handleDataReceived(int address)
{
    if(m_selNode != 0 && m_selNode->address() == address)
    {
        explorerTree_refresh_data(m_selNode->device());
    }
}

void QkExplorerWidget::_handleNodeUpdated(int address)
{
    if(m_selBoardType == sbtModuleDevice)
    {
        if(m_selNode != 0 && m_selNode->address() == address)
            _slotExplorerTrees_reload();
    }
}

void QkExplorerWidget::_handleSamplingModeChanged()
{
    bool enTriggeredMode, enNumberOfSamples;
    QkDevice::SamplingMode curSampMode = (QkDevice::SamplingMode)m_sampProp.mode->value().toInt();

    if(curSampMode == QkDevice::smTriggered)
        enTriggeredMode = true;
    else
        enTriggeredMode = false;
    if(curSampMode == QkDevice::smContinuous)
        enNumberOfSamples = false;
    else
        enNumberOfSamples = true;

    m_sampProp.N->setEnabled(enNumberOfSamples);
    m_sampProp.triggerClock->setEnabled(enTriggeredMode);
    m_sampProp.triggerScaler->setEnabled(enTriggeredMode);
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
    if(explorerList_findNode(address) >= 0)
        return;

    ui->explorerList->addItem(tr("Node") + QString().sprintf(" %04X", address));
    if(ui->explorerList->count() == 1)
    {
        ui->explorerList->setCurrentRow(0);
    }
}

void QkExplorerWidget::_slotExplorerTrees_reload()
{    
    if(ui->explorerList->currentRow() < 0)
        return;

    if(m_selBoardType == sbtModuleDevice)
    {
        if(m_selNode == 0)
            return;

        explorerTree_reload(etID_Module, m_selNode->module());
        if(m_selNode->module() != 0)
            explorerTree_refresh(etID_Module);

        explorerTree_reload(etID_Device, m_selNode->device());
        if(m_selNode->device() != 0)
            explorerTree_refresh(etID_Device);

    }
}

void QkExplorerWidget::explorerTree_reload(ExplorerTreeID id, QkBoard *board)
{
    QkDevice *device = 0;
    CPropertyBrowser *browser = 0;
    ExplorerTreeSel treeSel;

    browser = explorerTree_browser(id);
    if(browser == 0)
        return;

    int prevScrollValue = browser->verticalScrollBar()->value();
    browser->clear();

    treeSel = explorerTree_select(id);

    if(board == 0) // not available
    {
        CProperty *naProp = new CProperty(tr("(not available)"), CProperty::Label);
        browser->addProperty(naProp);
        return;
    }

    m_qkProp[treeSel].top = new CProperty("Qk", CProperty::Label);
    browser->addProperty(m_qkProp[treeSel].top);
    m_qkProp[treeSel].top->item()->setExpanded(true);

    m_qkProp[treeSel].version = new CProperty("Version", CProperty::Label, m_qkProp[treeSel].top);
    browser->addProperty(m_qkProp[treeSel].version, m_qkProp[treeSel].top);

    m_qkProp[treeSel].baudRate = new CProperty("Baud rate (bps)", CProperty::Label, m_qkProp[treeSel].top);
    browser->addProperty(m_qkProp[treeSel].baudRate, m_qkProp[treeSel].top);

    m_boardProp[treeSel].top = new CProperty("Board", CProperty::Label);
    browser->addProperty(m_boardProp[treeSel].top);
    m_boardProp[treeSel].top->item()->setExpanded(true);

    m_boardProp[treeSel].name = new CProperty("Name", CProperty::Text, m_boardProp[treeSel].top);
    browser->addProperty(m_boardProp[treeSel].name, m_boardProp[treeSel].top);

    m_boardProp[treeSel].fwVersion = new CProperty("Firmware", CProperty::Label, m_boardProp[treeSel].top);
    browser->addProperty(m_boardProp[treeSel].fwVersion, m_boardProp[treeSel].top);

    m_boardProp[treeSel].configs = new CProperty("Configuration", CProperty::Label);
    browser->addProperty(m_boardProp[treeSel].configs);
    m_boardProp[treeSel].configs->item()->setExpanded(true);


    m_boardProp[treeSel].configsList.clear();
    foreach(QkBoard::Config config, board->configs())
    {
        CProperty::Type propType;
        switch(config.type())
        {
        case QkBoard::Config::ctIntDec:
            propType = CProperty::Int;
            break;
        case QkBoard::Config::ctIntHex:
            propType = CProperty::Hex;
            break;
        case QkBoard::Config::ctFloat:
            propType = CProperty::Double;
            break;
        case QkBoard::Config::ctBool:
            propType = CProperty::Bool;
            break;
        case QkBoard::Config::ctDateTime:
            propType = CProperty::DateTime;
            break;
        case QkBoard::Config::ctTime:
            propType = CProperty::Time;
            break;
        default:
            propType = CProperty::Label;
        }

        CProperty *configProp = new CProperty(config.label(), propType, m_boardProp[treeSel].configs);
        configProp->setValue(config.value());
        browser->addProperty(configProp, m_boardProp[treeSel].configs);
        m_boardProp[treeSel].configsList.append(configProp);
    }

    if(etID_Device)
    {
        device = (QkDevice*) board;

        m_sampProp.top = new CProperty("Sampling", CProperty::Label);
        browser->addProperty(m_sampProp.top);
        m_sampProp.top->item()->setExpanded(true);

        m_sampProp.frequency = new CProperty("Frequency (Hz)", CProperty::Int, m_sampProp.top);
        browser->addProperty(m_sampProp.frequency, m_sampProp.top);

        m_sampProp.mode = new CProperty("Mode", CProperty::Enum, m_sampProp.top);
        QStringList samplingModes;
        samplingModes << "Single" << "Continuous" << "Triggered";
        m_sampProp.mode->setEnumList(samplingModes);
        browser->addProperty(m_sampProp.mode, m_sampProp.top);

        m_sampProp.N = new CProperty("N", CProperty::Int, m_sampProp.top);
        browser->addProperty(m_sampProp.N, m_sampProp.top);

        m_sampProp.triggerClock = new CProperty("Trigger clock", CProperty::Enum, m_sampProp.top);
        QStringList triggerClocks;
        triggerClocks << "1sec" << "10sec" << "1min" << "10min" << "1hour";

        m_sampProp.triggerClock->setEnumList(triggerClocks);
        browser->addProperty(m_sampProp.triggerClock, m_sampProp.top);

        m_sampProp.triggerScaler = new CProperty("Trigger scaler", CProperty::Int, m_sampProp.top);
        browser->addProperty(m_sampProp.triggerScaler, m_sampProp.top);

        m_deviceProp.data = new CProperty("Data", CProperty::Label);
        browser->addProperty(m_deviceProp.data);
        m_deviceProp.data->item()->setExpanded(true);

        m_deviceProp.dataList.clear();
        foreach(QkDevice::Data data, device->data())
        {
            CProperty *dataProp = new CProperty(data.label(), CProperty::Label, m_deviceProp.data);
            dataProp->setValue(data.value());
            browser->addProperty(dataProp, m_deviceProp.data);
            m_deviceProp.dataList.append(dataProp);
        }

        m_deviceProp.actions = new CProperty("Actions", CProperty::Label);
        browser->addProperty(m_deviceProp.actions);
        m_deviceProp.actions->item()->setExpanded(true);

        m_deviceProp.events = new CProperty("Events", CProperty::Label);
        browser->addProperty(m_deviceProp.events);
        m_deviceProp.events->item()->setExpanded(true);

        m_deviceProp.eventsList.clear();
        foreach(QkDevice::Event event, device->events())
        {
            CProperty *eventProp = new CProperty(event.label(), CProperty::Label, m_deviceProp.events);
            browser->addProperty(eventProp, m_deviceProp.events);
            m_deviceProp.eventsList.append(eventProp);
        }

        connect(m_sampProp.mode, SIGNAL(valueChanged(CProperty*)), this, SLOT(_handleSamplingModeChanged()));
        _handleSamplingModeChanged();
    }

    browser->verticalScrollBar()->setValue(prevScrollValue);
}

void QkExplorerWidget::explorerTree_refresh(ExplorerTreeID id, RefreshFlags flags)
{
    int i;
    QkBoard *selBoard = 0;
    QkDevice *selDevice = 0;
    CPropertyBrowser *browser = 0;
    ExplorerTreeSel treeSel;

    if((id == etID_Device || id == etID_Module) && m_selNode == 0)
        return;

    browser = explorerTree_browser(id);
    treeSel = explorerTree_select(id);

    switch(id)
    {
    case etID_Gateway:
        selBoard = m_conn->qk->gateway();
        break;
    case etID_Network:
        selBoard = m_conn->qk->network();
        break;
    case etID_Module:
        selBoard = m_selNode->module();
        break;
    case etID_Device:
        selDevice = m_selNode->device();
        selBoard = selDevice;
        break;
    default:
        selBoard = 0;
    }

    if(selBoard == 0)
        return;

    m_qkProp[treeSel].version->setValue(selBoard->qkInfo().versionString());
    m_qkProp[treeSel].baudRate->setValue(selBoard->qkInfo().baudRate);
    m_boardProp[treeSel].name->setValue(selBoard->name());
    m_boardProp[treeSel].fwVersion->setValue(QString().sprintf("%04X",selBoard->firmwareVersion()));


    QVector<QkBoard::Config> configs = selBoard->configs();
    for(i = 0; i < configs.count(); i++)
    {
        m_boardProp[treeSel].configsList[i]->setValue(configs[i].value());
    }


    if(id == etID_Device && selDevice != 0)
    {
        m_sampProp.frequency->setValue(selDevice->samplingInfo().frequency);
        m_sampProp.mode->setValue(selDevice->samplingInfo().mode);
        m_sampProp.triggerClock->setValue(selDevice->samplingInfo().triggerClock);
        m_sampProp.triggerScaler->setValue(selDevice->samplingInfo().triggerScaler);
        m_sampProp.N->setValue(selDevice->samplingInfo().N);

        /*QVector<QkDevice::Data> data = selDevice->data();
        for(i = 0; i < data.count(); i++)
        {
            m_deviceProp.dataList[i]->setValue(data[i].value());
        }*/
        explorerTree_refresh_data(selDevice);

        /*browser->clearChildren(m_deviceProp.data);
        foreach(QkDevice::Data data, selDevice->data())
        {
            CProperty *dataProp = new CProperty(data.label(), CProperty::Label, m_deviceProp.data);
            dataProp->setValue(data.value());
            browser->addProperty(dataProp, m_deviceProp.data);
        }*/

        /*browser->clearChildren(m_deviceProp.events);
        foreach(QkDevice::Event event, selDevice->events())
        {
            CProperty *eventProp = new CProperty(event.label(), CProperty::Label, m_deviceProp.events);
            browser->addProperty(eventProp, m_deviceProp.events);
        }*/
    }
}

void QkExplorerWidget::explorerTree_refresh_data(QkDevice *device)
{
    if(device == 0)
        return;

    int i;
    QString valueStr;
    QVector<QkDevice::Data> data = device->data();
    for(i = 0; i < data.count(); i++)
    {
        if(device->dataType() == QkDevice::Data::dtInt)
            valueStr = QString().sprintf("%-10d", (int)data[i].value());
        else
            valueStr = QString().sprintf("% .6f", data[i].value());
        m_deviceProp.dataList[i]->setValue(valueStr);
    }

    /*QList<QObject*> childrenObjects = m_deviceProp.data->children();

    return;

    int i = 0;
    foreach(QObject *childObject, childrenObjects)
    {
        QString valueStr;
        if(device->dataType() == QkDevice::Data::dtInt)
            valueStr = QString().sprintf("%-10d", (int)data[i].value());
        else
            valueStr = QString().sprintf("% .6f", data[i].value());

        CProperty *prop = (CProperty*)childObject;
        prop->setValue(valueStr);
        i++;
    }*/
}

CPropertyBrowser* QkExplorerWidget::explorerTree_browser(ExplorerTreeID id)
{
    switch(id)
    {
    case etID_Gateway:
    case etID_Module:
        //return ui->explorerTreeLeft;
    case etID_Network:
    case etID_Device:
        return ui->explorerTreeRight;
    }
    return 0;
}

QkExplorerWidget::ExplorerTreeSel QkExplorerWidget::explorerTree_select(ExplorerTreeID id)
{
    switch(id)
    {
    case etID_Gateway:
    case etID_Module:
        return etSel_Left;
    case etID_Network:
    case etID_Device:
        return etSel_Right;
    }
    return etSel_Right;
}

int QkExplorerWidget::explorerList_findNode(int address)
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
    ui->explorerList->clear();
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

void QkExplorerWidget::_slotUpdate()
{
    int i, address = 0;
    ExplorerTreeSel treeSel;

    if(m_selBoardType == sbtModuleDevice)
    {
        QkModule *selModule = m_selNode->module();
        QkDevice *selDevice = m_selNode->device();

        if(selDevice != 0)
        {
            address = selDevice->address();
            treeSel = explorerTree_select(etID_Device);

            selDevice->_setName(m_boardProp[treeSel].name->value().toString());

            for(i = 0; i < m_boardProp[treeSel].configsList.count(); i++)
            {
                CProperty *configProp = m_boardProp[treeSel].configsList.at(i);
                selDevice->setConfigValue(i, configProp->value());
            }

            QkDevice::SamplingInfo sampInfo;
            sampInfo.frequency = m_sampProp.frequency->value().toInt();
            sampInfo.mode = (QkDevice::SamplingMode) m_sampProp.mode->value().toInt();
            sampInfo.triggerClock = (QkDevice::TriggerClock) m_sampProp.triggerClock->value().toInt();
            sampInfo.triggerScaler = m_sampProp.triggerScaler->value().toInt();
            sampInfo.N = m_sampProp.N->value().toInt();
            selDevice->_setSamplingInfo(sampInfo);

            ui->statusBar->showMessage(tr("Updating..."));
            selDevice->update();
        }

        if(selModule != 0 || selDevice != 0)
        {
            m_conn->qk->getNode(address);
            ui->statusBar->showMessage(tr("Updated"), 2000);
        }
    }
}

void QkExplorerWidget::_slotClear()
{
    ui->logEventTable->removeAll();
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
    ui->update_button->setEnabled(enableButtons);
    ui->save_button->setEnabled(enableButtons);

    ui->clear_button->setEnabled(ui->explorerTabs->currentIndex() > 0);
}

void QkExplorerWidget::_slotDebug_log(int address, QString debugStr)
{
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

void QkExplorerWidget::_slotLogger_append(int address, QkDevice::Event event)
{
    int r = ui->logEventTable->addRow();

    QString timeStr = QTime::currentTime().toString("hh:mm:ss");
    QTableWidgetItem *time = new QTableWidgetItem();
    time->setText(timeStr);
    time->setTextAlignment(Qt::AlignCenter);
    ui->logEventTable->setItem(r, LoggerColumnEventTimestamp, time);

    QString srcStr = QString().sprintf("%04X", address);
    QTableWidgetItem *src = new QTableWidgetItem();
    src->setText(srcStr);
    src->setTextAlignment(Qt::AlignCenter);
    ui->logEventTable->setItem(r, LoggerColumnEventSource, src);

    QString labelStr = event.label();
    QTableWidgetItem *label = new QTableWidgetItem();
    label->setText(labelStr);
    label->setTextAlignment(Qt::AlignCenter);
    ui->logEventTable->setItem(r, LoggerColumnEventLabel, label);

    QString msgStr = event.message();

    QList<float> argsList = event.args();
    QString argsStr;
    argsStr.append("{ ");
    foreach(float arg, argsList)
        argsStr.append(QString::number(arg) + " ");
    argsStr.append("}");

    msgStr = insertArgsOnMessage(msgStr, event.args());

    QTableWidgetItem *msg = new QTableWidgetItem();
    msg->setText(msgStr);
    msg->setTextAlignment(Qt::AlignLeft);
    ui->logEventTable->setItem(r, LoggerColumnEventMessage, msg);

    QTableWidgetItem *args = new QTableWidgetItem();
    args->setText(argsStr);
    args->setTextAlignment(Qt::AlignLeft);
    ui->logEventTable->setItem(r, LoggerColumnEventArguments, args);

    ui->logEventTable->scrollToBottom();
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
