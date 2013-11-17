#include "boardpanel.h"
#include "ui_boardpanel.h"

#include "qkcore.h"

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

void BoardPanel::setBoard(QkBoard *board, QkBoard::Type type)
{
    switch(type)
    {
    case QkBoard::btDevice: ui->pixmap->setPixmap(QPixmap(":/img/device_text_color.png")); break;
    case QkBoard::btModule: ui->pixmap->setPixmap(QPixmap(":/img/comm_text_color.png")); break;
    default: ;
    }

    if(board == 0)
    {
        ui->label_name->setText(tr("n/a"));
        ui->label_firmware->setText(tr("n/a"));
        ui->label_qkversion->setText(tr("n/a"));
    }
    else
    {
        ui->label_name->setText(board->name());
        ui->label_firmware->setText(QString().sprintf("%04X", board->firmwareVersion()));
        ui->label_qkversion->setText(board->qkInfo().versionString());
    }
}
