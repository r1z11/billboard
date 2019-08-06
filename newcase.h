#ifndef NEWCASE_H
#define NEWCASE_H

#include <QDialog>
#include <QCompleter>

#include <dbfunctions.h>

namespace Ui {
class newcase;
}

class newcase : public QDialog
{
    Q_OBJECT

public:
    explicit newcase(QWidget *parent = 0);
    ~newcase();

private slots:
    void on_pushButton_cancel_clicked();

    void on_pushButton_save_clicked();

private:
    Ui::newcase *ui;

    DBfunctions db;
    QCompleter *clientCompleter, *pAreaCompleter;
};

#endif // NEWCASE_H
