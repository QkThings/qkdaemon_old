/*
 * QkThings LICENSE
 * The open source framework and modular platform for smart devices.
 * Copyright (C) 2014 <http://qkthings.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
    QkConnectionManager *m_connManager;
    QkDaemon *m_daemon;
    QkDaemonWidget *m_daemonWidget;
    QkExplorerWidget *m_explorerWidget;
    QToolBar *m_tools;
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayIconMenu;

    MessagesDialog *m_messagesDialog;
};

#endif // MAINWINDOW_H
