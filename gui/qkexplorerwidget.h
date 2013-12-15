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
    void _slotClear();
    void _slotBoardPanels_reload();
    void _slotExplorerList_reload();
    void _slotExplorerList_addNode(int address);
    void _handleExplorerListRowChanged(int row);
    void _handleDataReceived(int address);
    void _handleNodeUpdated(int address);
    void _slotLogger_append(int address, QkDevice::Event event);
    void _slotLogger_setEnabled(bool enabled);
    void _slotDebug_log(int address, QString debugStr);
    void _slotDebug_updateOptions();
    void _slotDebug_setEnabled(bool enabled);
    void showError(int code, int arg);
    void showError(const QString &message);
    void updateInterface();
    
private:
    enum StackedPanelIndex
    {
        spiNone = 0,
        spiNode = 1
    };
    enum SelectedBoardType
    {
        sbtGatewayNetwork,
        sbtModuleDevice
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

    void _setupLayout();
    void _setupConnections();
    int _explorerList_findNode(int address);

    QString insertArgsOnMessage(QString msg, QList<float> args);

    Ui::QkExplorerWidget *ui;
    QkConnection *m_conn;
    SelectedBoardType m_selBoardType;
    QkNode *m_selNode;

    bool m_debugPrintTime;
    bool m_debugPrintSource;
};

#endif // QKEXPLORERWIDGET_H
