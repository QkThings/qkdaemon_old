/*
 * QkThings LICENSE
 * The open source framework and modular platform for smart devices.
 * Copyright (C) 2014 <http://qkthings.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
    defaultBaudRates.append("54700");
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


    QString typeName = "typeName";
    QString param1 = "param1";
    QString param2 = "param2";

    switch(conn->descriptor().type)
    {
    case QkConnection::tSerial:
        typeName = "Serial";
        param1 = conn->descriptor().parameters["portName"].toString();
        param2 = conn->descriptor().parameters["baudRate"].toString();
        break;
    default: ;
    }

    QTableWidgetItem *itemType = new QTableWidgetItem(typeName);
    QTableWidgetItem *itemParam1 = new QTableWidgetItem(param1);
    QTableWidgetItem *itemParam2 = new QTableWidgetItem(param2);

    itemType->setTextAlignment(Qt::AlignCenter);

    table->setItem(row, ColumnConnType, itemType);
    table->setItem(row, ColumnParam1, itemParam1);
    table->setItem(row, ColumnParam2, itemParam2);

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
    qDebug() << __FUNCTION__;
//    pToolButton *button = reinterpret_cast<pToolButton*>(sender());

//    pTableWidget *table = ui->connectionsTable;
//    int r = table->currentRow();

//    QkConnection::Descriptor connDesc;

//    //connDesc.type = QkConnection::typeFromString(table->item(r, ColumnConnType)->text());
//    connDesc.params.append(table->item(r, ColumnParam1)->text());
//    connDesc.params.append(table->item(r, ColumnParam2)->text());

//    QkConnection *conn = m_connManager->connection(connDesc);
//    QSerialPort *sp;

//    qDebug() << "conn->descriptor().type" << conn->descriptor().type;
//    switch(conn->descriptor().type)
//    {
//    case QkConnection::ctSerial:
//        sp = (QSerialPort*)(conn->device());
//        if(sp->isOpen())
//        {
//            sp->close();
//            button->setText(tr("Disconnected"));
//        }
//        else
//        {
//            sp->setPortName(connDesc.params.at(0));
//            QSerialPort::BaudRate baudRate;
//            switch(connDesc.params.at(1).toInt())
//            {
//            case 9600: baudRate = QSerialPort::Baud9600; break;
//            case 38400: baudRate = QSerialPort::Baud38400; break;
//            case 115200: baudRate = QSerialPort::Baud115200; break;
//            default:
//                qDebug() << "Unable to set desired baud rate:" << connDesc.params.at(1);
//            }

//            sp->setBaudRate(baudRate);
//            if(sp->open(QIODevice::ReadWrite)) {
//                button->setText(tr("Connected"));
//            }
//            else
//                qDebug() << sp->errorString() << sp->portName() << sp->baudRate();
//        }
//        break;
//    default: ;
//    }

}

void QkConnectWidget::slotAddConnection()
{
    qDebug() << __FUNCTION__;
    QkConnection::Descriptor desc;

    switch(ui->connType_combo->currentIndex())
    {
    case 0: // serial
        desc.type = QkConnection::tSerial;
        desc.parameters.insert("portName", ui->serialPort_portName_combo->currentText());
        desc.parameters.insert("baudRate", ui->serialPort_baudRate_combo->currentText().toInt());
        break;
    default: ;
    }

    m_connManager->addConnection(desc);
}

void QkConnectWidget::slotRemoveConnection()
{
    qDebug() << __FUNCTION__;
    QkConnection::Descriptor desc;
    pTableWidget *table = ui->connectionsTable;
    desc = connectionDescriptor(table->currentRow());
    m_connManager->removeConnection(desc);
}

void QkConnectWidget::slotConnectionAdded(QkConnection *conn)
{
    qDebug() << __FUNCTION__;
    int row = ui->connectionsTable->rowCount();
    ui->connectionsTable->insertRow(row);
    fillRow(row, conn);
    ui->connectionsTable->updateToolButtonNumber();
    ui->connectionsTable->selectRow(row);
}

void QkConnectWidget::slotConnectionRemoved(QkConnection *conn)
{
    qDebug() << __FUNCTION__;
    int r = findConnection(conn->descriptor());
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
    QkConnection *conn = 0;

    //if(curRow != prevRow && curRow != -1)
    if(curRow != -1)
    {
        connDesc = connectionDescriptor(curRow);
        conn = m_connManager->connection(connDesc);
    }

    emit currentConnectionChanged(conn);

    updateInterface();
}

void QkConnectWidget::slotShowError(QString message)
{
    QMessageBox::critical(this, tr("Error"), message);
}

int QkConnectWidget::findConnection(const QkConnection::Descriptor &desc)
{
    pTableWidget *table = ui->connectionsTable;
    int row;
    for(row = 0; row < table->rowCount(); row++)
    {
        QkConnection::Descriptor rowConnDesc = connectionDescriptor(row);

        if(desc.type == rowConnDesc.type)
        {
            switch(rowConnDesc.type)
            {
            case QkConnection::tSerial:
                if(desc.parameters.keys().contains("portName"))
                    if(desc.parameters["portName"] == rowConnDesc.parameters["portName"])
                        return row;
                break;
            default: ;
            }
        }
    }

    return -1;
}

QkConnection::Descriptor QkConnectWidget::connectionDescriptor(int row)
{
    QkConnection::Descriptor desc;
    pTableWidget *table = ui->connectionsTable;

    if(row >= 0 || row < table->rowCount())
    {
        if(connectionType(row))
        {
            desc.type = QkConnection::tSerial;
            desc.parameters["portName"] = table->item(row, ColumnParam1)->text();
            desc.parameters["baudRate"] = table->item(row, ColumnParam2)->text().toInt();
        }
    }
    return desc;
}

QkConnection::Type QkConnectWidget::connectionType(int row)
{
    pTableWidget *table = ui->connectionsTable;
    if(row >= 0 || row < table->rowCount())
    {
        if(table->item(row, ColumnConnType)->text() == "Serial")
            return QkConnection::tSerial;
    }
    return QkConnection::tUnknown;
}

void QkConnectWidget::updateInterface()
{
    QkConnection::Descriptor connDesc;
    bool okToAdd = false;
    if(ui->connType_combo->currentIndex() == 0)  // Serial
    {
//        connDesc.type = QkConnection::ctSerial;
//        connDesc.params.append(ui->serialPort_portName_combo->currentText());
//        connDesc.params.append(ui->serialPort_baudRate_combo->currentText());
//        okToAdd = m_connManager->validate(connDesc);
        okToAdd = true;
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
