#include "messagesdialog.h"
#include "ui_messagesdialog.h"

#include "gui_globals.h"
#include <QColor>
#include <QTime>

MessagesDialog::MessagesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessagesDialog)
{
    ui->setupUi(this);

    ui->text_log->setFont(GUI_MONOFONT);

    setWindowTitle(GUI_MAINWINDOW_TITLE + " - Messages");
    setWindowFlags(Qt::Tool);
}

MessagesDialog::~MessagesDialog()
{
    delete ui;
}

void MessagesDialog::log(const QString &message, MessageType type)
{

    QColor textColor;
    switch(type)
    {
    //case mtInfo: textColor = Qt::blue; break;
    case mtError: textColor = Qt::red; break;
    default:
        textColor = Qt::black;
    }
    ui->text_log->setTextColor(textColor);

    QString text;

    if(type != mtBlank)
        text.append("[" + QTime::currentTime().toString("hh:mm:ss") + "] ");
    text.append(message);
    ui->text_log->append(text);
}
