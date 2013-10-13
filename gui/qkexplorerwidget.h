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
    void _slotUpdate();
    void _slotClear();
    void _slotExplorerTrees_reload();
    //void _slotExplorerTrees_refresh(int address);
    void _slotExplorerList_reload();
    void _slotExplorerList_addNode(int address);
    void _handleExplorerListRowChanged(int row);
    void _handleDataReceived(int address);
    void _handleNodeUpdated(int address);
    void _handleSamplingModeChanged();
    void _slotLogger_append(int address, QkDevice::Event event);
    void _slotDebug_log(int address, QString debugStr);
    void _slotDebug_updateOptions();
    void showError(int code, int arg);
    void showError(const QString &message);
    void updateInterface();
    
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
        //QList<CProperty*> actionsList;
        CProperty *events;
        QList<CProperty*> eventsList;
    };

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
    enum RefreshFlags //FIXME not used?
    {
        rfAll,
        rfData
    };
    enum
    {
        LoggerColumnEventTimestamp,
        LoggerColumnEventSource,
        LoggerColumnEventLabel,
        LoggerColumnEventMessage,
        LoggerColumnEventArguments
    };
    enum
    {
        LoggerColumnNotificationTimestamp,
        LoggerColumnNotificationSource,
        LoggerColumnNotificationMessage,
        LoggerColumnNotificationArguments
    };

    void setupLayout();
    void setupConnections();
    int explorerList_findNode(int address);
    void explorerTree_reload(ExplorerTreeID id, QkBoard *board);
    void explorerTree_refresh(ExplorerTreeID id, RefreshFlags flags = rfAll);
    void explorerTree_refresh_data(QkDevice *device);
    CPropertyBrowser* explorerTree_browser(ExplorerTreeID id);
    ExplorerTreeSel explorerTree_select(ExplorerTreeID id);

    QString insertArgsOnMessage(QString msg, QList<float> args);

    Ui::QkExplorerWidget *ui;
    QkConnection *m_conn;
    SelectedBoardType m_selBoardType;
    QkNode *m_selNode;
    QkProperties m_qkProp[etSel_COUNT];
    BoardProperties m_boardProp[etSel_COUNT];
    SamplingProperties m_sampProp;
    DeviceProperties m_deviceProp;

    bool m_debugPrintTime;
    bool m_debugPrintSource;
};

#endif // QKEXPLORERWIDGET_H
