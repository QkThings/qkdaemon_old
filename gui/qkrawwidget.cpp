#include "qkrawwidget.h"
#include "ui_qkrawwidget.h"

#include "qkconnect.h"

#include <QDebug>
#include <QTableWidgetItem>
#include <QTime>
#include <QFont>
#include <QHeaderView>

QkRawWidget::QkRawWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QkRawWidget)
{
    ui->setupUi(this);
    m_conn = 0;

    setupLayout();
    setupConnections();
}

QkRawWidget::~QkRawWidget()
{
    delete ui;
}

void QkRawWidget::setupLayout()
{
    QHeaderView *header;
#ifdef Q_OS_WIN
    ui->packetTable->setFont(QFont("Consolas", 9));
#endif

    header = ui->packetTable->horizontalHeader();
    header->setSectionResizeMode(ColumnPacketTime, QHeaderView::Fixed);
    header->setSectionResizeMode(ColumnPacketSource, QHeaderView::Fixed);
    header->setSectionResizeMode(ColumnPacketAddress, QHeaderView::Fixed);
    header->setSectionResizeMode(ColumnPacketCode, QHeaderView::Fixed);

    ui->packetTable->setColumnWidth(ColumnPacketTime, 70);
    ui->packetTable->setColumnWidth(ColumnPacketSource, 70);
    ui->packetTable->setColumnWidth(ColumnPacketAddress, 70);
    ui->packetTable->setColumnWidth(ColumnPacketCode, 100);


    ui->statusbar->hide();

    setWindowTitle("QkRaw");

}

void QkRawWidget::setupConnections()
{
    connect(ui->pushButton, SIGNAL(clicked()),
            this, SLOT(slotSendSearch()));

    connect(ui->clear_button, SIGNAL(clicked()),
            ui->packetTable, SLOT(removeAll()));
}

void QkRawWidget::setCurrentConnection(QkConnection *conn)
{
    if(m_conn != 0)
    {
        disconnect(m_conn, SIGNAL(incomingFrame(QByteArray)),
                   this, SLOT(slotIncomingFrame(QByteArray)));
    }

    m_conn = conn;
    connect(m_conn, SIGNAL(incomingFrame(QByteArray)),
            this, SLOT(slotIncomingFrame(QByteArray)));
}

void QkRawWidget::slotIncomingFrame(QByteArray frame)
{
    int r = ui->packetTable->addRow();
    Qk::Packet packet;
    Qk::PacketBuilder::parse(frame, &packet);

    QString timeStr = QTime::currentTime().toString("hh:mm:ss");
    QTableWidgetItem *time = new QTableWidgetItem();
    time->setText(timeStr);
    time->setTextAlignment(Qt::AlignCenter);
    ui->packetTable->setItem(r, ColumnPacketTime, time);

    QString sourceStr;
    switch(packet.source())
    {
    case QkBoard::btGateway:
        sourceStr = "Gateway"; break;
    case QkBoard::btNetwork:
        sourceStr = "Network"; break;
    case QkBoard::btModule:
        sourceStr = "Module"; break;
    case QkBoard::btDevice:
        sourceStr = "Device"; break;
    default: sourceStr = "???";
    }
    QTableWidgetItem *source = new QTableWidgetItem();
    source->setText(sourceStr);
    source->setTextAlignment(Qt::AlignHCenter);
    ui->packetTable->setItem(r, ColumnPacketSource, source);

    QString addrStr = QString().sprintf("%04X", packet.address);
    QTableWidgetItem *addr = new QTableWidgetItem();
    addr->setText(addrStr);
    addr->setTextAlignment(Qt::AlignCenter);
    ui->packetTable->setItem(r, ColumnPacketAddress, addr);

    QString codeStr = packet.codeFriendlyName();
    QTableWidgetItem *code = new QTableWidgetItem();
    code->setText(codeStr);
    code->setTextAlignment(Qt::AlignLeft);
    ui->packetTable->setItem(r, ColumnPacketCode, code);

    QString dataStr;
    int i;

    for(i=0; i< packet.data.count(); i++)
    {
        dataStr.append(QString().sprintf("%02X ",(quint8)packet.data.at(i)));
    }
    QTableWidgetItem *data = new QTableWidgetItem();
    data->setText(dataStr);
    ui->packetTable->setItem(r, ColumnPacketData, data);

    ui->packetTable->scrollToBottom();

}

void QkRawWidget::slotSendSearch()
{
    m_conn->qk.search();
}
