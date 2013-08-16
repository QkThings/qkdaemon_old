#ifndef QKRAWWIDGET_H
#define QKRAWWIDGET_H

#include <QMainWindow>

namespace Ui {
class QkRawWidget;
}

class QkConnection;
#include "qk.h"

class QkRawWidget : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit QkRawWidget(QWidget *parent = 0);
    ~QkRawWidget();

public slots:
    void setCurrentConnection(QkConnection *conn);
    
private slots:
    void slotIncomingFrame(QByteArray frame);
    void slotSendSearch();

private:
    enum ColumnPacket
    {
        ColumnPacketTime,
        ColumnPacketSource,
        ColumnPacketAddress,
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
};

#endif // QKRAWWIDGET_H
