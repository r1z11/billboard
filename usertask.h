#ifndef USERTASK_H
#define USERTASK_H

#include <QDialog>
#include <dbfunctions.h>
#include <QCompleter>

namespace Ui {
class UserTask;
}

class UserTask : public QDialog
{
    Q_OBJECT

public:
    explicit UserTask(QWidget *parent = 0);
    ~UserTask();

    void setTasknames(QStringList tasknames);

private slots:
    void on_pushButton_cancel_clicked();

    void on_pushButton_addUser_clicked();

private:
    Ui::UserTask *ui;

    DBfunctions db;
    QCompleter *userCompleter;
    QStringList tnames;

};

#endif // USERTASK_H
