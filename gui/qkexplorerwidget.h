#ifndef QKEXPLORERWIDGET_H
#define QKEXPLORERWIDGET_H

#include <QMainWindow>

namespace Ui {
class QkExplorerWidget;
}

#include "qkconnect.h"

class CProperty;
class CPropertyBrowser;

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
    void _slotExplorerTrees_reload();
    void _slotExplorerTrees_refresh();
    void _slotExplorerList_reload();
    void _slotExplorerList_addNode(int address);
    void _handleExplorerListRowChanged(int row);
    void showError(int code);
    void showError(const QString &message);
    
private:
    class QkProperties
    {
    public:
        CProperty *top;
        CProperty *version;
        CProperty *baudRate;
    };
    class BoardProperties
    {
    public:
        CProperty *top;
        CProperty *name;
        CProperty *fwVersion;
        CProperty *configs;
        QList<CProperty*> configsList;
    };
    class SamplingProperties
    {
    public:
        CProperty *top;
        CProperty *frequency;
        CProperty *mode;
        CProperty *triggerClock;
        CProperty *triggerScaler;
        CProperty *N;
    };
    class DeviceProperties
    {
    public:
        CProperty *data;
        QList<CProperty*> dataList;
        CProperty *actions;
        QList<CProperty*> actionsList;
        CProperty *events;
        QList<CProperty*> eventsList;
    } ;

    enum SelectedBoardType
    {
        sbtGatewayNetwork,
        sbtModuleDevice
    };
    enum ExplorerTreeID
    {
        etID_Module,
        etID_Device,
        etID_Gateway,
        etID_Network
    };
    enum ExplorerTreeSel
    {
        etSel_Left,
        etSel_Right,
        etSel_COUNT
    };
    enum
    {
        ExplorerTreeColumnElement,
        ExplorerTreeColumnValue,
        ExplorerTreeColumnPopup
    };
    enum RefreshFlags
    {
        rfAll,
        rfData
    };

    void setupLayout();
    void setupConnections();
    void explorerTree_reload(ExplorerTreeID id, bool na = false);
    void explorerTree_refresh(ExplorerTreeID id, RefreshFlags flags = rfAll);
    CPropertyBrowser* explorerTree_browser(ExplorerTreeID id);
    ExplorerTreeSel explorerTree_select(ExplorerTreeID id);

    void updateInterface();

    Ui::QkExplorerWidget *ui;
    QkConnection *m_conn;
    SelectedBoardType m_selBoardType;
    QkNode *m_selNode;
    QkProperties m_qkProp[etSel_COUNT];
    BoardProperties m_boardProp[etSel_COUNT];
    SamplingProperties m_sampProp;
    DeviceProperties m_deviceProp;
};

#endif // QKEXPLORERWIDGET_H
