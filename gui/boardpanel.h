#ifndef BOARDPANEL_H
#define BOARDPANEL_H

#include <QWidget>

namespace Ui {
class BoardPanel;
}

class BoardPanel : public QWidget
{
    Q_OBJECT
    
public:
    explicit BoardPanel(QWidget *parent = 0);
    ~BoardPanel();
    
private:
    Ui::BoardPanel *ui;
};

#endif // BOARDPANEL_H
