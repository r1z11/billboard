#ifndef TRUSTTRANSACTION_H
#define TRUSTTRANSACTION_H

#include <QDialog>
#include <QCompleter>
#include <dbfunctions.h>

namespace Ui {
class TrustTransaction;
}

class TrustTransaction : public QDialog
{
    Q_OBJECT

public:
    explicit TrustTransaction(QWidget *parent = 0);
    ~TrustTransaction();

    void setupDeposit();
    void setupWithdrawal();

private slots:
    void on_pushButton_cancel_clicked();

    void on_pushButton_save_clicked();

    void on_comboBox_client_activated(const QString &arg1);

    void on_comboBox_client_currentIndexChanged(const QString &arg1);

    void on_comboBox_client_currentTextChanged(const QString &arg1);

    void on_lineEdit_tAmount_editingFinished();

private:
    Ui::TrustTransaction *ui;

    bool isDeposit;
    void getClientBalance();
    DBfunctions db;
    QCompleter *cCompleter;
};

#endif // TRUSTTRANSACTION_H
