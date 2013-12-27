#include "qkexplorerwidget.h"
#include "ui_qkexplorerwidget.h"

#include "qkcore.h"
#include "cproperty.h"
#include "cpropertybrowser.h"
#include "eventwidget.h"
#include "rtplot.h"
#include "plotsettings.h"
#include "ui_plotsettings.h"

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
#include <QVBoxLayout>
#include <QLayout>

QkExplorerWidget::QkExplorerWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QkExplorerWidget),
    m_conn(0)
{
    m_debugPrintTime = false;
    m_debugPrintSource = false;

    setupLayout();
    setupConnections();
    setup();
    updateInterface();
}

QkExplorerWidget::~QkExplorerWidget()
{
    delete ui;
}

void QkExplorerWidget::setup()
{
    ui->plottingWidget->setLayout(new QVBoxLayout);
    QLayout *layout = ui->plottingWidget->layout();
    layout->setMargin(0);
    layout->setSpacing(0);
    slotViewer_addPlot();
}

void QkExplorerWidget::setupLayout()
{
    ui->setupUi(this);
    ui->menubar->hide();
    ui->statusBar->hide();

#ifdef Q_OS_WIN
    ui->debugText->setFont(QFont("Consolas",9));
#else
    ui->debugText->setFont(QFont("Monospace",9));
#endif

    setWindowTitle("QkExplorer");
    updateInterface();
}

void QkExplorerWidget::setupConnections()
{
    connect(ui->search_button, SIGNAL(clicked()),
            this, SLOT(slotSearch()));
    connect(ui->start_button, SIGNAL(clicked()),
            this, SLOT(slotStart()));
    connect(ui->stop_button, SIGNAL(clicked()),
            this, SLOT(slotStop()));
    connect(ui->button_clearLogger, SIGNAL(clicked()),
            ui->eventTable, SLOT(removeAll()));
    connect(ui->check_enableLogger, SIGNAL(clicked(bool)),
            this, SLOT(slotLogger_setEnabled(bool)));
    connect(ui->button_clearDebug, SIGNAL(clicked()),
            ui->debugText, SLOT(clear()));
    connect(ui->check_enableDebug, SIGNAL(clicked(bool)),
            this, SLOT(slotDebug_setEnabled(bool)));

    connect(ui->explorerList, SIGNAL(currentRowChanged(int)),
            this, SLOT(slotExplorerListRowChanged(int)));
    connect(ui->explorerList, SIGNAL(currentRowChanged(int)),
            this, SLOT(slotBoardPanels_reload()));

    connect(ui->check_enableLogger, SIGNAL(clicked()),
            this, SLOT(updateInterface()));
    connect(ui->check_enableDebug, SIGNAL(clicked()),
            this, SLOT(updateInterface()));

    connect(ui->debugPrintTime_check, SIGNAL(clicked()),
            this, SLOT(slotDebug_updateOptions()));
    connect(ui->debugPrintSource_check, SIGNAL(clicked()),
            this, SLOT(slotDebug_updateOptions()));

    connect(ui->explorerTabs, SIGNAL(currentChanged(int)),
            this, SLOT(updateInterface()));

    connect(ui->viewer_buttonAddPlot, SIGNAL(clicked()),
            this, SLOT(slotViewer_addPlot()));

    connect(ui->viewer_buttonPlotSettings, SIGNAL(clicked()),
            this, SLOT(slotViewer_showHideSettings()));

    connect(ui->viewer_comboPlot, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotViewer_currentPlotChanged(int)));

    PlotSettings *plotSettings = ui->plotSettings;
    connect(plotSettings->ui->comboNode, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(slotViewer_nodeChanged(QString)));

    connect(plotSettings->ui->buttonAddWaveform, SIGNAL(clicked()),
            this, SLOT(slotViewer_addWaveform()));
}

void QkExplorerWidget::setCurrentConnection(QkConnection *conn)
{
    if(m_conn != 0)
    {
        disconnect(m_conn->qk, SIGNAL(error(int,int)), this, SLOT(showError(int,int)));
        disconnect(m_conn->qk, SIGNAL(deviceFound(int)), this, SLOT(slotNodeFound(int)));
        disconnect(m_conn->qk, SIGNAL(deviceUpdated(int)), this, SLOT(slotNodeUpdated(int)));
        disconnect(m_conn->qk, SIGNAL(dataReceived(int)), this, SLOT(slotDataReceived(int)));
        disconnect(m_conn->qk, SIGNAL(eventReceived(int,QkDevice::Event)), this, SLOT(slotLogger_append(int,QkDevice::Event)));
        disconnect(m_conn->qk, SIGNAL(debugString(int,QString)), this, SLOT(slotDebug_log(int,QString)));
    }

    m_conn = conn;
    connect(m_conn->qk, SIGNAL(error(int,int)), this, SLOT(showError(int,int)));
    connect(m_conn->qk, SIGNAL(deviceFound(int)), this, SLOT(slotNodeFound(int)));
    connect(m_conn->qk, SIGNAL(deviceUpdated(int)), this, SLOT(slotNodeUpdated(int)));
    connect(m_conn->qk, SIGNAL(dataReceived(int)), this, SLOT(slotDataReceived(int)));
    connect(m_conn->qk, SIGNAL(eventReceived(int,QkDevice::Event)), this, SLOT(slotLogger_append(int,QkDevice::Event)));
    connect(m_conn->qk, SIGNAL(debugString(int,QString)), this, SLOT(slotDebug_log(int,QString)));

    updateInterface();
}

void QkExplorerWidget::slotExplorerListRowChanged(int row)
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


void QkExplorerWidget::slotDataReceived(int address)
{
    qDebug() << "slotDataReceived()";
    if(m_selNode != 0 && m_selNode->address() == address)
    {
        ui->deviceBoardPanel->refreshData();
    }

    foreach(AddressDataPair *addrDataPair, m_waveformMapper.keys())
    {
        if(addrDataPair->address == address)
        {
            Waveform *wf = m_waveformMapper.value(addrDataPair);
            RTPlot *plot = m_plotMapper.value(wf);
            QVector<QkDevice::Data> data = m_conn->qk->node(address)->device()->data();
            plot->addData(data[addrDataPair->dataIdx].value(), wf);
        }
    }
}

void QkExplorerWidget::slotNodeFound(int address)
{
    qDebug() << "slotNodeFound()";

    QString addrStr = QString().sprintf("%04X", address);

    if(explorerList_findNode(address) < 0)
    {
        ui->explorerList->addItem(tr("Node ") + addrStr);
        if(ui->explorerList->count() == 1)
            ui->explorerList->setCurrentRow(0);
    }

    PlotSettings *plotSettings = ui->plotSettings;
    plotSettings->ui->comboNode->addItem(addrStr);
}


void QkExplorerWidget::slotNodeUpdated(int address)
{
    if(m_selBoardType == sbtModuleDevice)
    {
        if(m_selNode != 0 && m_selNode->address() == address)
            slotBoardPanels_reload();
    }
}

void QkExplorerWidget::slotExplorerList_reload() //FIXME is it really needed?
{
    ui->explorerList->clear();
    QList<int> addressList = m_conn->qk->nodes().keys();
    foreach(int address, addressList)
    {
        slotNodeFound(address);
    }
}

void QkExplorerWidget::slotBoardPanels_reload()
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

void QkExplorerWidget::slotSearch()
{
    int stackIdx = ui->stackedPanels->currentIndex();
    ui->stackedPanels->setCurrentIndex(stackIdx);
    ui->explorerList->clear();

    PlotSettings *plotSettings = ui->plotSettings;
    plotSettings->ui->comboNode->clear();

    m_conn->qk->search();
}

void QkExplorerWidget::slotStart()
{
    m_conn->qk->start();
    foreach(RTPlotDock *plotDock, m_plotDockMapper.values())
        plotDock->plot()->start();
}

void QkExplorerWidget::slotStop()
{
    m_conn->qk->stop();
    foreach(RTPlotDock *plotDock, m_plotDockMapper.values())
        plotDock->plot()->stop();
}

void QkExplorerWidget::slotClear()
{
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
}

void QkExplorerWidget::slotDebug_log(int address, QString debugStr)
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

void QkExplorerWidget::slotDebug_updateOptions()
{
    m_debugPrintTime = ui->debugPrintTime_check->isChecked();
    m_debugPrintSource = ui->debugPrintSource_check->isChecked();
}

void QkExplorerWidget::slotDebug_setEnabled(bool enabled)
{
    if(enabled)
        ui->label_debugEnabled->setPixmap(QPixmap(":/icons/on_16.png"));
    else
        ui->label_debugEnabled->setPixmap(QPixmap(":/icons/off_16.png"));
}

void QkExplorerWidget::slotLogger_append(int address, QkDevice::Event event)
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

void QkExplorerWidget::slotLogger_setEnabled(bool enabled)
{
    if(enabled)
        ui->label_loggerEnabled->setPixmap(QPixmap(":/icons/on_16.png"));
    else
        ui->label_loggerEnabled->setPixmap(QPixmap(":/icons/off_16.png"));
}

void QkExplorerWidget::slotViewer_addPlot()
{
    QLayout *layout = ui->plottingWidget->layout();
    RTPlotDock *plotDock = createPlotDock();
    layout->addWidget(plotDock);
    ui->viewer_comboPlot->addItem(plotDock->windowTitle(), plotDock->id());
    ui->viewer_comboPlot->setCurrentIndex(ui->viewer_comboPlot->count()-1);
}

void QkExplorerWidget::slotViewer_addWaveform()
{   
    qDebug() << "slotViewer_addWaveform()";
    int currentPlotIdx = ui->viewer_comboPlot->currentIndex();
    int plotDockID = ui->viewer_comboPlot->itemData(currentPlotIdx).toInt();
    RTPlotDock *plotDock = m_plotDockMapper.value(plotDockID);
    RTPlot *plot = plotDock->plot();

    PlotSettings *plotSettings = ui->plotSettings;
    QString addrStr = plotSettings->ui->comboNode->currentText();
    bool ok;
    int addr = addrStr.toInt(&ok, 16);

    int dataIdx = plotSettings->ui->comboData->currentIndex();
    if(dataIdx >= 0)
    {
        AddressDataPair *addrDataPair = new AddressDataPair;
        addrDataPair->address = addr;
        addrDataPair->dataIdx = dataIdx;

        QkNode *node = m_conn->qk->node(addr);
        QkDevice::Data data = node->device()->data().at(dataIdx);

        Waveform *wf = plot->addWaveform(data.label());
        m_waveformMapper.insert(addrDataPair, wf);
        m_plotMapper.insert(wf, plot);
    }

    plotSettings->setCurrentPlotDock(plotDock);
}

void QkExplorerWidget::slotViewer_nodeChanged(QString addrStr)
{
    PlotSettings *plotSettings = ui->plotSettings;
    bool ok;
    int addr = addrStr.toInt(&ok, 16);
    QkNode *node = m_conn->qk->node(addr);
    foreach(QkDevice::Data data, node->device()->data())
        plotSettings->ui->comboData->addItem(data.label());
}

void QkExplorerWidget::slotViewer_dockSelected(int  id)
{
    int i;
    for(i=0; i < ui->viewer_comboPlot->count(); i++)
    {
        int plotDockID = ui->viewer_comboPlot->itemData(i).toInt();
        if(plotDockID == id)
            ui->viewer_comboPlot->setCurrentIndex(i);
    }

}

void QkExplorerWidget::slotViewer_currentPlotChanged(int idx)
{
    m_currentPlotDock = 0;

    if(ui->viewer_comboPlot->count() > 0)
    {
        int plotDockID = ui->viewer_comboPlot->itemData(idx).toInt();
        m_currentPlotDock = m_plotDockMapper.value(plotDockID);
    }

    if(m_currentPlotDock != 0)
        ui->plotSettings->setCurrentPlotDock(m_currentPlotDock);
}

void QkExplorerWidget::slotViewer_showHideSettings()
{
    if(ui->plotSettings->isVisible())
    {
        ui->plotSettings->hide();
        ui->viewer_checkGlobal->hide();
    }
    else
    {
        ui->plotSettings->show();
        ui->viewer_checkGlobal->show();
    }
}

RTPlotDock *QkExplorerWidget::createPlotDock()
{
    RTPlotDock *plotDock = new RTPlotDock(new RTPlot, this);
    connect(plotDock, SIGNAL(dockSelected(int)), this, SLOT(slotViewer_dockSelected(int)));
    m_plotDockMapper.insert(plotDock->id(), plotDock);
    return plotDock;
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
