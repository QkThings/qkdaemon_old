#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QkExplorerWidget;
class QkConnectWidget;
class QkConnect;
class QkCore;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void slotShowExplorer();
    void logMessage(QString text);
    
private:
    void setupLayout();
    void setupConnections();

    Ui::MainWindow *ui;
    QkCore *m_qk;
    QkConnect *m_conn;
    QkConnectWidget *m_connectWidget;
    QkExplorerWidget *m_explorerWidget;

};

#endif // MAINWINDOW_H
