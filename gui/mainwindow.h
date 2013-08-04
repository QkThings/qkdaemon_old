#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QkExplorerWidget;
class QkDaemonWidget;
class QkConnect;
class QkCore;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void slotShowHideExplorer();
    void logMessage(QString text);
    
private:
    void setupLayout();
    void setupConnections();

    Ui::MainWindow *ui;
    QkCore *m_qk;
    QkConnect *m_connect;
    QkDaemonWidget *m_daemonWidget;
    QkExplorerWidget *m_explorerWidget;
    QToolBar *m_tools;

};

#endif // MAINWINDOW_H
