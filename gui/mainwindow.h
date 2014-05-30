#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QSystemTrayIcon;

class QkExplorerWidget;
class QkDaemonWidget;
class QkDaemon;
class QkConnectionManager;

#include "messagesdialog.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void slotShowHideInfo();
    void slotShowHideSettings();
    void slotShowHideExplorer();
    void log(const QString &message, MessagesDialog::MessageType type = MessagesDialog::mtInfo);
    
private slots:
    void handleDaemonStatusMessage(QString message);

private:
    void setupLayout();
    void setupConnections();

    Ui::MainWindow *ui;
    QkConnectionManager *m_connect;
    QkDaemon *m_daemon;
    QkDaemonWidget *m_daemonWidget;
    QkExplorerWidget *m_explorerWidget;
    QToolBar *m_tools;
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayIconMenu;

    MessagesDialog *m_messagesDialog;
};

#endif // MAINWINDOW_H
