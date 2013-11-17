#ifndef BOARDPANEL_H
#define BOARDPANEL_H

#include <QWidget>

namespace Ui {
class BoardPanel;
}

#include "qkcore.h"

class BoardPanel : public QWidget
{
    Q_OBJECT
    
public:
    explicit BoardPanel(QWidget *parent = 0);
    ~BoardPanel();
    
    void setBoard(QkBoard *board, QkBoard::Type type);

private:
    Ui::BoardPanel *ui;
};

#endif // BOARDPANEL_H
