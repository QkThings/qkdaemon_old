#ifndef QKEXPLORERWIDGET_H
#define QKEXPLORERWIDGET_H

#include <QMainWindow>

namespace Ui {
class QkExplorerWidget;
}

#include "qkconnect.h"

class QkExplorerWidget : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit QkExplorerWidget(QWidget *parent = 0);
    ~QkExplorerWidget();

public slots:
    void setCurrentConnection(QkConnection *conn);

private slots:
    void _slotSearch();
    void _slotStart();
    void _slotStop();
    void _slotInitExplorerTrees();
    void showError(int code);
    void showError(const QString &message);
    
private:
    enum ExplorerTreeID {
        etID_Module,
        etID_Device,
        etID_Gateway,
        etID_Network
    };
    enum {
        ExplorerTreeTopGateway,
        ExplorerTreeTopNetwork,
        ExplorerTreeTopNodes
    };
    enum {
        ExplorerTreeColumnElement,
        ExplorerTreeColumnValue
    };

    void setupLayout();
    void setupConnections();
    void explorerTree_init(ExplorerTreeID id);
    //void explorerTree_createNode();
    //void explorerTree_deleteNode();

    void explorerTree_updateGateway();
    void explorerTree_updateNetwork();
    void explorerTree_updateNodes();
    void explorerTree_updateNode(int address = 0);

    void updateInterface();

    Ui::QkExplorerWidget *ui;
    QkConnection *m_conn;
};

#endif // QKEXPLORERWIDGET_H
