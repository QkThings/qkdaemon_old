#include "qkexplorerwidget.h"
#include "ui_qkexplorerwidget.h"

QkExplorerWidget::QkExplorerWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QkExplorerWidget)
{
    ui->setupUi(this);
    ui->menubar->hide();
    ui->statusbar->hide();

    setWindowTitle("QkExplorer");
}

QkExplorerWidget::~QkExplorerWidget()
{
    delete ui;
}
