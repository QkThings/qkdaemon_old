#ifndef MESSAGESDIALOG_H
#define MESSAGESDIALOG_H

#include <QDialog>

namespace Ui {
class MessagesDialog;
}

class MessagesDialog : public QDialog
{
    Q_OBJECT
    
public:
    enum MessageType
    {
        mtBlank,
        mtInfo,
        mtError
    };
    explicit MessagesDialog(QWidget *parent = 0);
    ~MessagesDialog();

public slots:
    void log(const QString &message, MessageType type = mtInfo);
    
private:
    Ui::MessagesDialog *ui;
};

#endif // MESSAGESDIALOG_H
