#include "qkexplorerwidget.h"
#include "ui_qkexplorerwidget.h"

#include "qk.h"

#include <QDebug>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>

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

    QHeaderView *header = ui->explorerTree->header();
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->explorerTree->setIndentation(5);

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
}

void QkExplorerWidget::setCurrentConnection(QkConnection *conn)
{
    qDebug() << "QkExplorerWidget::setCurrentConnection()";
    m_conn = conn;
    explorerTree_init();
    explorerTree_updateGateway();
    explorerTree_updateNetwork();
    explorerTree_updateNodes();

    updateInterface();
}

void QkExplorerWidget::explorerTree_init()
{
    QTreeWidget *tree = ui->explorerTree;
    tree->clear();
    if(m_conn == 0)
        return;
    QkCore *qk = &(m_conn->qk);

    QStringList strings;
    QTreeWidgetItem *item;

    strings.clear();
    strings << "Gateway";
    if(qk->gateway() == 0)
        strings << "(n/a)";
    item = new QTreeWidgetItem(strings);
    tree->addTopLevelItem(item);

    strings.clear();
    strings << "Network";
    if(qk->network() == 0)
        strings << "(n/a)";
    item = new QTreeWidgetItem(strings);
    tree->addTopLevelItem(item);

    strings.clear();
    strings << "Nodes";
    strings << QString::number(qk->nodes().count());
    item = new QTreeWidgetItem(strings);
    tree->addTopLevelItem(item);
}

void QkExplorerWidget::explorerTree_updateGateway()
{

}

void QkExplorerWidget::explorerTree_updateNetwork()
{

}

void QkExplorerWidget::explorerTree_updateNodes()
{

}

void QkExplorerWidget::explorerTree_updateNode(int address)
{
    (void)address;
}

void QkExplorerWidget::_slotSearch()
{
    m_conn->qk.search();
}

void QkExplorerWidget::_slotStart()
{
    m_conn->qk.start();
}

void QkExplorerWidget::_slotStop()
{
    m_conn->qk.stop();
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
