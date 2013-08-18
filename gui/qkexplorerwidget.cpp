#include "qkexplorerwidget.h"
#include "ui_qkexplorerwidget.h"

#include "qk.h"
#include "cproperty.h"
#include "cpropertybrowser.h"

#include <QDebug>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QMessageBox>

QkExplorerWidget::QkExplorerWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QkExplorerWidget),
    m_conn(0)
{
    setupLayout();
    setupConnections();
}

QkExplorerWidget::~QkExplorerWidget()
{
    delete ui;
}

void QkExplorerWidget::setupLayout()
{
    ui->setupUi(this);
    ui->menubar->hide();
    ui->statusbar->hide();

    QHeaderView *header;

    header = ui->explorerTreeLeft->header();
    header->setSectionResizeMode(ExplorerTreeColumnElement, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(ExplorerTreeColumnValue, QHeaderView::Stretch);
    header->setSectionResizeMode(ExplorerTreeColumnPopup, QHeaderView::Fixed);

    header = ui->explorerTreeRight->header();
    header->setSectionResizeMode(ExplorerTreeColumnElement, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(ExplorerTreeColumnValue, QHeaderView::Stretch);
    header->setSectionResizeMode(ExplorerTreeColumnPopup, QHeaderView::Fixed);

    ui->explorerTreeLeft->setColumnWidth(ExplorerTreeColumnPopup, 16);
    ui->explorerTreeRight->setColumnWidth(ExplorerTreeColumnPopup, 16);

    ui->explorerTreeLeft->setColumnHidden(ExplorerTreeColumnPopup, true);
    ui->explorerTreeRight->setColumnHidden(ExplorerTreeColumnPopup, true);

    ui->explorerTreeLeft->setIndentation(10);
    ui->explorerTreeRight->setIndentation(10);

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
    connect(ui->explorerList, SIGNAL(currentRowChanged(int)),
            this, SLOT(_handleExplorerListRowChanged(int)));

    connect(ui->explorerList, SIGNAL(currentRowChanged(int)),
            this, SLOT(_slotExplorerTrees_reload()));
}

void QkExplorerWidget::setCurrentConnection(QkConnection *conn)
{
    if(m_conn != 0)
    {
        disconnect(m_conn->qk, SIGNAL(error(int)), this, SLOT(showError(int)));
        disconnect(m_conn->qk, SIGNAL(deviceDetected(int)), this, SLOT(_slotExplorerList_addNode(int)));
    }

    m_conn = conn;
    connect(m_conn->qk, SIGNAL(error(int)), this, SLOT(showError(int)));
    connect(m_conn->qk, SIGNAL(deviceDetected(int)), this, SLOT(_slotExplorerList_addNode(int)));

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

        if(m_selNode->module() == 0)
            explorerTree_reload(etID_Module, true);
        else
        {
            explorerTree_reload(etID_Module);
            //explorerTree_refresh(etID_Module);
        }
        if(m_selNode->device() == 0)
            explorerTree_reload(etID_Device, true);
        else
        {
            explorerTree_reload(etID_Device);
            explorerTree_refresh(etID_Device);
        }
    }
}

void QkExplorerWidget::explorerTree_reload(ExplorerTreeID id, bool na)
{
    CPropertyBrowser *browser = 0;
    ExplorerTreeSel treeSel;

    browser = explorerTree_browser(id);
    if(browser == 0)
        return;
    browser->clear();

    treeSel = explorerTree_select(id);

    if(na) // not available
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

    if(etID_Device)
    {
        m_sampProp.top = new CProperty("Sampling", CProperty::Label);
        browser->addProperty(m_sampProp.top);
        m_sampProp.top->item()->setExpanded(true);

        m_sampProp.frequency = new CProperty("Frequency (Hz)", CProperty::Int, m_sampProp.top);
        browser->addProperty(m_sampProp.frequency, m_sampProp.top);

        m_sampProp.mode = new CProperty("Mode", CProperty::Enum, m_sampProp.top);
        QStringList samplingModes;
        samplingModes << "Continuous" << "Triggered";
        m_sampProp.mode->setEnumList(samplingModes);
        browser->addProperty(m_sampProp.mode, m_sampProp.top);

        m_sampProp.triggerClock = new CProperty("Trigger clock", CProperty::Enum, m_sampProp.top);
        QStringList triggerClocks;
        triggerClocks << "Single" << "1sec" << "10sec" << "1min" << "10min" << "1hour";
        m_sampProp.triggerClock->setEnumList(triggerClocks);
        browser->addProperty(m_sampProp.triggerClock, m_sampProp.top);

        m_sampProp.triggerScaler = new CProperty("Trigger scaler", CProperty::Int, m_sampProp.top);
        browser->addProperty(m_sampProp.triggerScaler, m_sampProp.top);

        m_sampProp.N = new CProperty("N", CProperty::Int, m_sampProp.top);
        browser->addProperty(m_sampProp.N, m_sampProp.top);

        m_deviceProp.data = new CProperty("Data", CProperty::Label);
        browser->addProperty(m_deviceProp.data);
        m_deviceProp.data->item()->setExpanded(true);

        m_deviceProp.actions = new CProperty("Actions", CProperty::Label);
        browser->addProperty(m_deviceProp.actions);
        m_deviceProp.actions->item()->setExpanded(true);

        m_deviceProp.events = new CProperty("Events", CProperty::Label);
        browser->addProperty(m_deviceProp.events);
        m_deviceProp.events->item()->setExpanded(true);
    }
}

void QkExplorerWidget::_slotExplorerTrees_refresh() //FIXME remove this?
{
    if(m_selBoardType == sbtModuleDevice)
    {

        /*QkModule *module = m_selNode->module();
        if(module != 0)
        {

        }

        QkDevice *device = m_selNode->device();
        if(device != 0)
        {

        }*/
    }
}

void QkExplorerWidget::explorerTree_refresh(ExplorerTreeID id, RefreshFlags flags)
{
    qDebug() << "explorerTree_refresh()";
    QkBoard *selBoard = 0;
    QkDevice *selDevice = 0;
    CPropertyBrowser *browser;
    ExplorerTreeSel treeSel;

    int count;

    if((id == etID_Device || id == etID_Module) && m_selNode == 0)
        return;

    browser = explorerTree_browser(id);
    treeSel = explorerTree_select(id);

    qDebug() << "m_conn" << m_conn;
    qDebug() << "m_selNode" << m_selNode;

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

    qDebug() << "selBoard" << selBoard;

    if(selBoard == 0)
        return;

    m_qkProp[treeSel].version->setValue(selBoard->qkInfo().versionString());
    m_qkProp[treeSel].baudRate->setValue(selBoard->qkInfo().baudRate);
    m_boardProp[treeSel].name->setValue(selBoard->name());
    m_boardProp[treeSel].fwVersion->setValue(QString().sprintf("%04X",selBoard->firmwareVersion()));

    browser->clearChildren(m_boardProp[treeSel].configs);
    foreach(QkBoard::Config config, selBoard->configs())
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
    }


    if(id == etID_Device && selDevice != 0)
    {
        m_sampProp.frequency->setValue(selDevice->samplingInfo().frequency);
        m_sampProp.mode->setValue(selDevice->samplingInfo().mode);
        m_sampProp.triggerClock->setValue(selDevice->samplingInfo().triggerClock);
        m_sampProp.triggerScaler->setValue(selDevice->samplingInfo().triggerScaler);
        m_sampProp.N->setValue(selDevice->samplingInfo().N);
    }

}

CPropertyBrowser* QkExplorerWidget::explorerTree_browser(ExplorerTreeID id)
{
    switch(id)
    {
    case etID_Gateway:
    case etID_Module:
        return ui->explorerTreeLeft;
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

void QkExplorerWidget::updateInterface()
{
    bool enableButtons = false;
    if(m_conn == 0)
    {
        enableButtons = false;
    }
    else
    {
        enableButtons = true;
    }

    ui->search_button->setEnabled(enableButtons);
    ui->start_button->setEnabled(enableButtons);
    ui->stop_button->setEnabled(enableButtons);
    ui->update_button->setEnabled(enableButtons);
    ui->save_button->setEnabled(enableButtons);
}

void QkExplorerWidget::showError(int code)
{
    showError(QkCore::errorMessage(code));
}

void QkExplorerWidget::showError(const QString &message)
{
    QMessageBox::critical(this, tr("Error"), message);
}
