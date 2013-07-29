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
    //setupConnections();
    slotReloadAvailableSerialPorts();
    updateInterface();
}

QkConnectWidget::~QkConnectWidget()
{
    delete ui;
}

void QkConnectWidget::setQkConnect(QkConnect *conn)
{
    m_connect = conn;
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
    QHeaderView *header = table->horizontalHeader();
    header->setSectionResizeMode(ColumnOpenClose , QHeaderView::ResizeToContents);
    header->setSectionResizeMode(ColumnConnType  , QHeaderView::Fixed);
    header->setSectionResizeMode(ColumnParam1    , QHeaderView::Fixed);
    header->setSectionResizeMode(ColumnParam2    , QHeaderView::Stretch);

    header->setSortIndicatorShown(false);

    table->setColumnWidth(ColumnConnType, 80);
    table->setColumnWidth(ColumnParam1, 80);

    table->setAlternatingRowColors(true);
}

void QkConnectWidget::setupConnections()
{
    connect(m_connect, SIGNAL(error(QString)),
            this, SLOT(slotShowError(QString)));
    connect(ui->reload_button, SIGNAL(clicked()),
            this, SLOT(slotReloadAvailableSerialPorts()));
    connect(ui->add_button, SIGNAL(clicked()),
            this, SLOT(slotAddConnection()));
    connect(ui->remove_button, SIGNAL(clicked()),
            this, SLOT(slotRemoveConnection()));
    connect(m_connect, SIGNAL(connectionAdded(QkConnect::Connection*)),
            this, SLOT(slotConnectionAdded(QkConnect::Connection*)));
    connect(m_connect, SIGNAL(connectionRemoved(QkConnect::Connection*)),
            this, SLOT(slotConnectionRemoved(QkConnect::Connection*)));
}

void QkConnectWidget::fillRow(int row, QkConnect::Connection *c)
{
    qDebug() << "fillRow";
    pTableWidget *table = ui->connectionsTable;
    QString connTypeName;

    switch(c->type)
    {
    case QkConnect::ctSerial: connTypeName = "Serial Port"; break;
    case QkConnect::ctTCP: connTypeName = "TCP/IP"; break;
    default: connTypeName = "???";
    }

    QTableWidgetItem *connType = new QTableWidgetItem(connTypeName);
    QTableWidgetItem *param1 = new QTableWidgetItem(c->param1);
    QTableWidgetItem *param2 = new QTableWidgetItem(c->param2);

    pToolButton *openCloseButton = new pToolButton(this);
    openCloseButton->setAutoRaise(true);
    openCloseButton->setContextMenuPolicy(Qt::NoContextMenu);
    openCloseButton->setText(tr("Close"));

    connect(openCloseButton, SIGNAL(pressed()), table, SLOT(updateSelectedRowFromToolButton()));
    //connect(openCloseButton, SIGNAL(clicked()), this, SLOT(viewDatasheetHandler()));

    table->setCellWidget(row, ColumnOpenClose, openCloseButton);

    table->setItem(row, ColumnConnType, connType);
    table->setItem(row, ColumnParam1, param1);
    table->setItem(row, ColumnParam2, param2);
}

void QkConnectWidget::slotAddConnection()
{
    if(ui->connType_combo->currentIndex() == 0)  // Serial
    {
        m_connect->addConnection(QkConnect::ctSerial,
                                 ui->serialPort_portName_combo->currentText(),
                                 ui->serialPort_baudRate_combo->currentText());
    }
}

void QkConnectWidget::slotRemoveConnection()
{

}

void QkConnectWidget::slotConnectionAdded(QkConnect::Connection *c)
{
    qDebug() << "slotConnectionAdded";
    int row = ui->connectionsTable->rowCount();
    ui->connectionsTable->insertRow(row);
    fillRow(row, c);
    //ui->connectionsTable->updateToolButtonNumber();
}

void QkConnectWidget::slotConnectionRemoved(QkConnect::Connection *c)
{

}

void QkConnectWidget::slotReloadAvailableSerialPorts()
{
    QStringList availablePorts;
    qDebug() << "Available serial ports:";
    foreach(QSerialPortInfo sp, QSerialPortInfo::availablePorts())
    {
        //FIXME Show all virtual serial ports
        if(!sp.portName().contains("USB"))
            continue;

        qDebug() << "Port name:    " << sp.portName();
        if(sp.hasVendorIdentifier())
            qDebug() << "Vendor ID:    " << sp.vendorIdentifier();
        if(sp.hasProductIdentifier())
            qDebug() << "Product ID:   " << sp.productIdentifier();
        availablePorts.append(sp.portName());
    }
    ui->serialPort_portName_combo->clear();
    ui->serialPort_portName_combo->addItems(availablePorts);
}

void QkConnectWidget::slotShowError(QString message)
{
    QMessageBox::critical(this, tr("Error"), message);
}

void QkConnectWidget::updateInterface()
{
    bool okToAdd = false;
    if(ui->connType_combo->currentIndex() == 0)  // Serial
    {
        okToAdd = m_connect->validate(QkConnect::ctSerial,
                                      ui->serialPort_portName_combo->currentText(),
                                      ui->serialPort_baudRate_combo->currentText());
    }
    ui->add_button->setEnabled(okToAdd);
}
