#include "qkrawwidget.h"
#include "ui_qkrawwidget.h"

#include "qkconnect.h"

#include <QDebug>
#include <QTableWidgetItem>
#include <QTime>
#include <QFont>
#include <QHeaderView>
#include <QEventLoop>

QkRawWidget::QkRawWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QkRawWidget)
{
    ui->setupUi(this);
    m_conn = 0;
    m_printASCII = false;

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
    header->setSectionResizeMode(ColumnPacketAddress, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(ColumnPacketFlags, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(ColumnPacketCode, QHeaderView::Fixed);
    header->setSectionResizeMode(ColumnPacketData, QHeaderView::ResizeToContents);

    ui->packetTable->setColumnWidth(ColumnPacketTime, 70);
    ui->packetTable->setColumnWidth(ColumnPacketSource, 70);
    //ui->packetTable->setColumnWidth(ColumnPacketAddress, 60);
    //ui->packetTable->setColumnWidth(ColumnPacketFlags, 50);
    ui->packetTable->setColumnWidth(ColumnPacketCode, 100);

    //ui->packetTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->packetTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    header->setMinimumWidth(4096); //FIXME this is a workarround to have the horizontal scrollbar always on

    ui->statusbar->hide();

    setWindowTitle("QkRaw");

}

void QkRawWidget::setupConnections()
{
    connect(ui->pushButton, SIGNAL(clicked()),
            this, SLOT(slotSendSearch()));

    connect(ui->clear_button, SIGNAL(clicked()),
            ui->packetTable, SLOT(removeAll()));

    connect(ui->ascii_checkBox, SIGNAL(clicked(bool)),
            this, SLOT(slotPrintASCII(bool)));
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
void QkRawWidget::slotIncomingFrame(Qk::Frame frame)
{
    return;

    int i;
    QStringList sl;

    /*for(i=0; i<frame.count(); i++)
        sl.append(QString().sprintf("%02X", frame.at(i) & 0xFF));
    qDebug() << "Frame:" << sl;*/

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
    case QkBoard::btComm:
        sourceStr = "Comm"; break;
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

    QString flagsStr = QString().sprintf("%04X", packet.flags);
    QTableWidgetItem *flags = new QTableWidgetItem();
    flags->setText(flagsStr);
    flags->setTextAlignment(Qt::AlignCenter);
    ui->packetTable->setItem(r, ColumnPacketFlags, flags);

    QString codeStr = packet.codeFriendlyName();
    QTableWidgetItem *code = new QTableWidgetItem();
    code->setText(codeStr);
    code->setTextAlignment(Qt::AlignLeft);
    ui->packetTable->setItem(r, ColumnPacketCode, code);

    QString dataStr;
    for(i=0; i< packet.data.count(); i++)
    {
        if(!m_printASCII)
            dataStr.append(QString().sprintf("%02X ",(quint8)packet.data.at(i)));
        else
            dataStr.append(QString().sprintf("%2c ",(quint8)packet.data.at(i)));
    }
    QTableWidgetItem *data = new QTableWidgetItem();
    data->setText(dataStr);
    ui->packetTable->setItem(r, ColumnPacketData, data);

    ui->packetTable->scrollToBottom();
}

void QkRawWidget::slotSendSearch()
{
    m_conn->qk()->search();
}

void QkRawWidget::slotPrintASCII(bool enabled)
{
    m_printASCII = enabled;
}
