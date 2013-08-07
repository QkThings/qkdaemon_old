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
    
private:
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
    void explorerTree_init();
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
