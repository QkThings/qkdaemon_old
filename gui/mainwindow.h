#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QkRawWidget;
class QkExplorerWidget;
class QkDaemonWidget;
class QkDaemon;
class QkConnect;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    enum LogMessageType
    {
        lmtBlank,
        lmtInfo,
        lmtError
    };

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void slotShowHideSettings();
    void slotShowHideExplorer();
    void slotShowHideRaw();
    void logMessage(QString text, LogMessageType lmt = lmtBlank);
    
private slots:
    void _handleDaemonStatusMessage(QString message);

private:
    void setupLayout();
    void setupConnections();

    Ui::MainWindow *ui;
    QkConnect *m_connect;
    QkDaemon *m_daemon;
    QkDaemonWidget *m_daemonWidget;
    QkExplorerWidget *m_explorerWidget;
    QkRawWidget *m_rawWidget;
    QToolBar *m_tools;
};

#endif // MAINWINDOW_H
