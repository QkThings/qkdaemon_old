#include "boardpanel.h"
#include "ui_boardpanel.h"

BoardPanel::BoardPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BoardPanel)
{
    ui->setupUi(this);
}

BoardPanel::~BoardPanel()
{
    delete ui;
}
