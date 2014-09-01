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

#include <QWidget>

namespace Ui {
class QkConnectWidget;
}

#include "qkconnect.h"

class pToolButton;

class QkConnectWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit QkConnectWidget(QWidget *parent = 0);
    ~QkConnectWidget();

    void setQkConnect(QkConnectionManager *conn);

signals:
    void currentConnectionChanged(QkConnection *conn);

public slots:
    void slotAddConnection();
    void slotRemoveConnection();
    void slotConnectionAdded(QkConnection *conn);
    void slotConnectionRemoved(QkConnection *conn);
    void slotReloadAvailableSerialPorts();
    void slotOpenCloseConnection();
    void slotCurrentCellChanged(int curRow, int curCol, int prevRow, int prevCol);
    void slotShowError(QString message);

private slots:
    void updateInterface();
    
private:
    enum {
        ColumnConnType,
        ColumnParam1,
        ColumnParam2,
        ColumnOpenClose,
        ColumnMaxCount
    };

    void setupLayout();
    void setupConnections();
    void fillRow(int row, QkConnection *conn);
    int findConnection(const QkConnection::Descriptor &desc);
    QkConnection::Descriptor connectionDescriptor(int row);
    QkConnection::Type connectionType(int row);

    Ui::QkConnectWidget *ui;
    QkConnectionManager *m_connManager;
};

#endif // QKCONNECTWIDGET_H
