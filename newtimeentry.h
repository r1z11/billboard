#ifndef NEWTIMEENTRY_H
#define NEWTIMEENTRY_H

#include <QDialog>
#include <QCompleter>
#include "dbfunctions.h"

namespace Ui {
class NewTimeEntry;
}

class NewTimeEntry : public QDialog
{
    Q_OBJECT

public:
    explicit NewTimeEntry(QWidget *parent = 0);
    ~NewTimeEntry();

    void setTime(double hrs, double mins);

private slots:
    void on_radioButton_billable_toggled(bool checked);

    void on_radioButton_nonBillable_toggled(bool checked);

    void on_checkBox_caseLink_toggled(bool checked);

    void on_comboBox_rate_currentIndexChanged(const QString &arg1);

    void on_pushButton_calcFee_clicked();

    void on_lineEdit_hrs_editingFinished();

    void on_lineEdit_mins_editingFinished();

    void on_pushButton_cancel_clicked();

    void on_pushButton_save_clicked();

    void on_comboBox_clients_activated(const QString &arg1);

    void on_comboBox_cases_activated(const QString &arg1);

private:
    Ui::NewTimeEntry *ui;

    DBfunctions db;

    QCompleter *casecompleter, *clientscompleter, *taskcompleter;

    void calculateFee();
};

#endif // NEWTIMEENTRY_H
