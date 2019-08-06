#ifndef NEWEXPENSE_H
#define NEWEXPENSE_H

#include <QDialog>
#include <QCompleter>
#include <dbfunctions.h>

namespace Ui {
class NewExpense;
}

class NewExpense : public QDialog
{
    Q_OBJECT

public:
    explicit NewExpense(QWidget *parent = 0);
    ~NewExpense();

private slots:
    void on_pushButton_cancel_clicked();

    void on_pushButton_submit_clicked();

    void on_comboBox_client_activated(const QString &arg1);

private:
    Ui::NewExpense *ui;

    DBfunctions db;
    QCompleter *userCompleter, *clientCompleter, *caseCompleter;
    QSqlQueryModel *casesModel;
};

#endif // NEWEXPENSE_H
