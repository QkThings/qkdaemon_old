#ifndef QKEXPLORERWIDGET_H
#define QKEXPLORERWIDGET_H

#include <QMainWindow>

namespace Ui {
class QkExplorerWidget;
}

#include "qkconnect.h"

class CProperty;
class CPropertyBrowser;
class RTPlotDock;
class RTPlot;
class Waveform;

class QkExplorerWidget : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit QkExplorerWidget(QWidget *parent = 0);
    ~QkExplorerWidget();

public slots:
    void setCurrentConnection(QkConnection *conn);

private slots:
    void slotSearch();
    void slotStart();
    void slotStop();
    void slotClear();
    void slotBoardPanels_reload();
    void slotExplorerList_reload();
    void slotNodeFound(int address);
    void slotExplorerListRowChanged(int row);
    void slotDataReceived(int address);
    void slotNodeUpdated(int address);

    void slotLogger_append(int address, QkDevice::Event event);
    void slotLogger_setEnabled(bool enabled);

    void slotDebug_log(int address, QString debugStr);
    void slotDebug_updateOptions();
    void slotDebug_setEnabled(bool enabled);

    void slotViewer_addPlot();
    void slotViewer_addWaveform();
    void slotViewer_nodeChanged(QString addrStr);
    void slotViewer_dockSelected(int id);
    void slotViewer_currentPlotChanged(int idx);
    void slotViewer_plotTitleChanged(int id, QString title);
    RTPlotDock* createPlotDock();

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

    class AddressDataPair {
    public:
        int address;
        int dataIdx;
    };

    void setup();
    void setupLayout();
    void setupConnections();
    int explorerList_findNode(int address);

    QString insertArgsOnMessage(QString msg, QList<float> args);

    Ui::QkExplorerWidget *ui;
    QkConnection *m_conn;
    SelectedBoardType m_selBoardType;
    QkNode *m_selNode;

    QMap<int,RTPlotDock*> m_plotDockMapper;
    QMap<Waveform*,RTPlot*> m_plotMapper;
    QMap<AddressDataPair*, Waveform*> m_waveformMapper;

    RTPlotDock *m_currentPlotDock;


    bool m_debugPrintTime;
    bool m_debugPrintSource;
};

#endif // QKEXPLORERWIDGET_H
