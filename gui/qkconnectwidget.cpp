#include "qkconnectwidget.h"
#include "ui_qkconnectwidget.h"

#include "qkconnect.h"

#include <QDebug>
#include <QMessageBox>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>

QkConnectWidget::QkConnectWidget(QkConnect *conn, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QkConnectWidget),
    m_conn(conn)
{
    ui->setupUi(this);

    setupLayout();
    setupConnections();
}

QkConnectWidget::~QkConnectWidget()
{
    delete ui;
}

void QkConnectWidget::setupConnections()
{
    connect(ui->refreshSerial_toolButton, SIGNAL(clicked()),
            this, SLOT(slotRefreshSerialPorts()));
    connect(ui->add_toolButton, SIGNAL(clicked()),
            this, SLOT(slotAddConnection()));
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

    ui->baudRate_comboBox->addItems(defaultBaudRates);
    ui->baudRate_comboBox->setCurrentText("115200");

    slotRefreshSerialPorts();
}

void QkConnectWidget::slotRefreshSerialPorts()
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
    ui->portName_comboBox->clear();
    ui->portName_comboBox->addItems(availablePorts);

    slotUpdateInterface();
}

void QkConnectWidget::slotAddConnection()
{
    if(ui->connType_comboBox->currentIndex() == 0)  // Serial
    {
        m_conn->addConnection(QkConnect::ctSerial,
                              ui->portName_comboBox->currentText(),
                              ui->baudRate_comboBox->currentText());
    }
}

void QkConnectWidget::slotUpdateInterface()
{
    bool okToAdd = false;
    if(ui->connType_comboBox->currentIndex() == 0)  // Serial
    {
        okToAdd= m_conn->validate(QkConnect::ctSerial,
                                  ui->portName_comboBox->currentText(),
                                  ui->baudRate_comboBox->currentText());
    }
    ui->add_toolButton->setEnabled(okToAdd);
}
