#ifndef NEWPAYMENT_H
#define NEWPAYMENT_H

#include <QDialog>
#include <dbfunctions.h>

namespace Ui {
class NewPayment;
}

class NewPayment : public QDialog
{
    Q_OBJECT

public:
    explicit NewPayment(QWidget *parent = 0);
    ~NewPayment();

private slots:
    void on_pushButton_cancel_clicked();

    void on_comboBox_client_activated(const QString &arg1);

    void on_comboBox_case_activated(const QString &arg1);

    void on_comboBox_invno_activated(const QString &arg1);

    void on_pushButton_pay_clicked();

    void on_comboBox_client_currentTextChanged(const QString &arg1);

private:
    Ui::NewPayment *ui;

    DBfunctions db;
    void getClientCases(QString clientname);
};

#endif // NEWPAYMENT_H
