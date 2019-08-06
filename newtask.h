#ifndef NEWTASK_H
#define NEWTASK_H

#include <QDialog>
#include <dbfunctions.h>
#include <QCompleter>
#include <QDebug>

namespace Ui {
class newtask;
}

class newtask : public QDialog
{
    Q_OBJECT

public:
    explicit newtask(QWidget *parent = 0);
    ~newtask();

private slots:
    void on_pushButton_addTask_clicked();

    void on_pushButton_cancel_clicked();

    void on_checkBox_caseLink_toggled(bool checked);

private:
    Ui::newtask *ui;

    DBfunctions db;
    QCompleter *caseCompleter, *clientCompleter;
};

#endif // NEWTASK_H
