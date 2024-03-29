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

#ifndef QKDAEMONWIDGET_H
#define QKDAEMONWIDGET_H

#include <QWidget>

namespace Ui {
class QkDaemonWidget;
}

class QkDaemon;
class QkConnectionManager;
class QkExplorerWidget;

class QkDaemonWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit QkDaemonWidget(QkDaemon *daemon, QWidget *parent = 0);
    ~QkDaemonWidget();

public slots:

private slots:
    void slotConnect();
    void updateInterface();

private:
    void setupConnections();
    void setupLayout();

    Ui::QkDaemonWidget *ui;

    QkDaemon *m_daemon;
    QkConnectionManager *m_conn;
    bool m_connected;
};

#endif // QKDAEMONWIDGET_H
