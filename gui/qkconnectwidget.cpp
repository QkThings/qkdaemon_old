#include "qkconnectwidget.h"
#include "ui_qkconnectwidget.h"

#include "qkconnect.h"
#include "ptoolbutton.h"
#include "ptablewidget.h"

#include <QDebug>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
QkConnectWidget::QkConnectWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QkConnectWidget)
{
    ui->setupUi(this);

    setupLayout();
    slotReloadAvailableSerialPorts();
    updateInterface();
}

QkConnectWidget::~QkConnectWidget()
{
    delete ui;
}

void QkConnectWidget::setQkConnect(QkConnectionManager *conn)
{
    m_connManager = conn;
    setupConnections();
}

void QkConnectWidget::setupLayout()
{
    QStringList defaultBaudRates;
    defaultBaudRates.append("9600");
    defaultBaudRates.append("38400");
    defaultBaudRates.append("115200");
    /*foreach(qint32 baud, QSerialPortInfo::standardBaudRates())
    {
        defaultBaudRates.append(QString::number(baud));
    }*/

    ui->serialPort_baudRate_combo->addItems(defaultBaudRates);
    ui->serialPort_baudRate_combo->setCurrentText("38400");


    QTableWidget *table = ui->connectionsTable;
    table->setColumnCount(4);
    table->setFrameStyle(QFrame::NoFrame);
    QHeaderView *header = table->horizontalHeader();
    header->setSectionResizeMode(ColumnOpenClose , QHeaderView::Fixed);
    header->setSectionResizeMode(ColumnConnType  , QHeaderView::Fixed);
    header->setSectionResizeMode(ColumnParam1    , QHeaderView::Fixed);
    header->setSectionResizeMode(ColumnParam2    , QHeaderView::Stretch);

    header->setSortIndicatorShown(false);

    table->setColumnWidth(ColumnConnType, 80);
    table->setColumnWidth(ColumnParam1, 80);
    table->setColumnWidth(ColumnOpenClose, 100);

    table->setColumnHidden(ColumnOpenClose, true);

    table->setAlternatingRowColors(true);

//    table->hideColumn(ColumnOpenClose);
}

void QkConnectWidget::setupConnections()
{
    connect(m_connManager, SIGNAL(error(QString)),
            this, SLOT(slotShowError(QString)));
    connect(ui->reload_button, SIGNAL(clicked()),
            this, SLOT(slotReloadAvailableSerialPorts()));
    connect(ui->add_button, SIGNAL(clicked()),
            this, SLOT(slotAddConnection()));
    connect(ui->remove_button, SIGNAL(clicked()),
            this, SLOT(slotRemoveConnection()));
    connect(m_connManager, SIGNAL(connectionAdded(QkConnection*)),
            this, SLOT(slotConnectionAdded(QkConnection*)));
    connect(m_connManager, SIGNAL(connectionRemoved(QkConnection*)),
            this, SLOT(slotConnectionRemoved(QkConnection*)));
    connect(ui->connectionsTable, SIGNAL(currentCellChanged(int,int,int,int)),
            this, SLOT(slotCurrentCellChanged(int,int,int,int)));
}

void QkConnectWidget::fillRow(int row, QkConnection *conn)
{
    qDebug() << "fillRow";
    int i, col;
    pTableWidget *table = ui->connectionsTable;

    QTableWidgetItem *connType = new QTableWidgetItem(QkConnection::typeToString(conn->descriptor.type));
    table->setItem(row, ColumnConnType, connType);

    QTableWidgetItem *param;
    for(i = 0, col = ColumnParam1; i < conn->descriptor.params.count() && col < ColumnMaxCount; i++, col++)
    {
        param = new QTableWidgetItem(conn->descriptor.params.at(i));
        table->setItem(row, col, param);
    }

    pToolButton *openCloseButton = new pToolButton(this);
    openCloseButton->setAutoRaise(true);
    openCloseButton->setContextMenuPolicy(Qt::NoContextMenu);
    openCloseButton->setText(tr("Connected"));

    connect(openCloseButton, SIGNAL(pressed()), table, SLOT(updateSelectedRowFromToolButton()));
    connect(openCloseButton, SIGNAL(clicked()), this, SLOT(slotOpenCloseConnection()));

    table->setCellWidget(row, ColumnOpenClose, openCloseButton);
}

void QkConnectWidget::slotOpenCloseConnection()
{
    pToolButton *button = reinterpret_cast<pToolButton*>(sender());

    pTableWidget *table = ui->connectionsTable;
    int r = table->currentRow();

    QkConnection::Descriptor connDesc;

    connDesc.type = QkConnection::typeFromString(table->item(r, ColumnConnType)->text());
    connDesc.params.append(table->item(r, ColumnParam1)->text());
    connDesc.params.append(table->item(r, ColumnParam2)->text());

    QkConnection *conn = m_connManager->connection(connDesc);

    if(conn == 0) {
        return;
    }

    QSerialPort *sp;

    switch(conn->descriptor.type)
    {
    case QkConnection::ctSerial:
        sp = (QSerialPort*)(conn->device);
        if(sp->isOpen())
        {
            sp->close();
            button->setText(tr("Disconnected"));
        }
        else
        {
            sp->setPortName(connDesc.params.at(0));
            sp->setBaudRate(QSerialPort::Baud38400);
            if(sp->open(QIODevice::ReadWrite))
                button->setText(tr("Connected"));
            else
                qDebug() << sp->errorString() << sp->portName() << sp->baudRate();
        }
        break;
    default: ;
    }

}

void QkConnectWidget::slotAddConnection()
{
    QkConnection::Descriptor connDesc;
    if(ui->connType_combo->currentIndex() == 0)  // Serial
    {
        connDesc.type = QkConnection::ctSerial;
        connDesc.params.append(ui->serialPort_portName_combo->currentText());
        connDesc.params.append(ui->serialPort_baudRate_combo->currentText());
        m_connManager->addConnection(connDesc);
    }
}

void QkConnectWidget::slotRemoveConnection()
{

    QkConnection::Descriptor connDesc;
    pTableWidget *table = ui->connectionsTable;
    connDesc = connectionDescriptor(table->currentRow());
    m_connManager->removeConnection(connDesc);
}

void QkConnectWidget::slotConnectionAdded(QkConnection *conn)
{
    qDebug() << "slotConnectionAdded";
    int row = ui->connectionsTable->rowCount();
    ui->connectionsTable->insertRow(row);
    fillRow(row, conn);
    ui->connectionsTable->updateToolButtonNumber();
    ui->connectionsTable->selectRow(row);
}

void QkConnectWidget::slotConnectionRemoved(QkConnection *conn)
{
    qDebug() << "slotConnectionRemoved" << conn;
    int r = findConnection(conn->descriptor);
    qDebug() << "remove row" << r;
    ui->connectionsTable->removeRow(r);
}

void QkConnectWidget::slotReloadAvailableSerialPorts()
{
    QStringList availablePorts;
    qDebug() << "Available serial ports:";
    foreach(QSerialPortInfo sp, QSerialPortInfo::availablePorts())
    {
        //FIXME Show all virtual serial ports
#ifdef Q_OS_UNIX
        if(!sp.portName().contains("USB") && !sp.portName().contains("ACM"))
            continue;
#endif

        qDebug() << "Port name:    " << sp.portName();
        /*if(sp.hasVendorIdentifier())
            qDebug() << "Vendor ID:    " << sp.vendorIdentifier();
        if(sp.hasProductIdentifier())
            qDebug() << "Product ID:   " << sp.productIdentifier();*/
        availablePorts.append(sp.portName());
    }
    ui->serialPort_portName_combo->clear();
    ui->serialPort_portName_combo->addItems(availablePorts);

    updateInterface();
}

void QkConnectWidget::slotCurrentCellChanged(int curRow, int curCol, int prevRow, int prevCol)
{
    (void)curCol;
    (void)prevCol;

    QkConnection::Descriptor connDesc;
    if(curRow != prevRow && curRow != -1)
    {
        connDesc = connectionDescriptor(curRow);
        QkConnection *conn = m_connManager->connection(connDesc);
        if(conn != 0)
        {
            emit currentConnectionChanged(conn);
        }
    }

    updateInterface();
}

void QkConnectWidget::slotShowError(QString message)
{
    QMessageBox::critical(this, tr("Error"), message);
}

int QkConnectWidget::findConnection(const QkConnection::Descriptor &connDesc)
{
    pTableWidget *table = ui->connectionsTable;
    int r;
    for(r = 0; r < table->rowCount(); r++)
    {
        if(table->item(r, ColumnConnType)->text() == QkConnection::typeToString(connDesc.type) &&
           table->item(r, ColumnParam1)->text() == connDesc.params.at(0))
        {
            return r;
        }
    }

    return -1;
}

QkConnection::Descriptor QkConnectWidget::connectionDescriptor(int row)
{
    QkConnection::Descriptor connDesc;
    pTableWidget *table = ui->connectionsTable;

    if(row >= 0 || row < table->rowCount())
    {
        connDesc.type = QkConnection::typeFromString(table->item(row, ColumnConnType)->text());
        connDesc.params.append(table->item(row, ColumnParam1)->text());
        connDesc.params.append(table->item(row, ColumnParam2)->text());
    }
    return connDesc;
}

void QkConnectWidget::updateInterface()
{
    QkConnection::Descriptor connDesc;
    bool okToAdd = false;
    if(ui->connType_combo->currentIndex() == 0)  // Serial
    {
        connDesc.type = QkConnection::ctSerial;
        connDesc.params.append(ui->serialPort_portName_combo->currentText());
        connDesc.params.append(ui->serialPort_baudRate_combo->currentText());
        okToAdd = m_connManager->validate(connDesc);
    }
    ui->add_button->setEnabled(okToAdd);

    if(ui->connectionsTable->currentRow() >= 0)
    {
        ui->remove_button->setEnabled(true);
    }
    else
    {
        ui->remove_button->setEnabled(false);
    }
}
