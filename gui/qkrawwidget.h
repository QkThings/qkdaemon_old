#ifndef QKRAWWIDGET_H
#define QKRAWWIDGET_H

#include <QMainWindow>

namespace Ui {
class QkRawWidget;
}

class QkConnection;
#include "qkcore.h"

class QkRawWidget : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit QkRawWidget(QWidget *parent = 0);
    ~QkRawWidget();

public slots:
    void setCurrentConnection(QkConnection *conn);
    
private slots:
    void slotIncomingFrame(Frame frame);
    void slotSendSearch();
    void slotPrintASCII(bool enabled);

private:
    enum ColumnPacket
    {
        ColumnPacketTime,
        ColumnPacketSource,
        ColumnPacketAddress,
        ColumnPacketFlags,
        ColumnPacketCode,
        ColumnPacketData
    };
    enum ColumnFrame
    {
        ColumnFrameTime,
        ColumnFrameData
    };

    void setupLayout();
    void setupConnections();

    Ui::QkRawWidget *ui;
    QkConnection *m_conn;
    bool m_printASCII;
};

#endif // QKRAWWIDGET_H
