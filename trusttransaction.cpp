#include "trusttransaction.h"
#include "ui_trusttransaction.h"

TrustTransaction::TrustTransaction(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TrustTransaction)
{
    ui->setupUi(this);

    db.db = QSqlDatabase::database("bill");

    //clients
    QSqlQueryModel *cmodel = new QSqlQueryModel();
    cmodel->setQuery("SELECT DISTINCT clientname FROM clients WHERE archived = '0' ", db.db);
    cCompleter = new QCompleter(cmodel, this);
    cCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->comboBox_client->setCompleter(cCompleter);
    ui->comboBox_client->setModel(cmodel);

    //payment methods
    QStringList paytypes;
    paytypes << "Cash" << "Cheque" << "Electronic transfer";
    ui->comboBox_payMethod->addItems(paytypes);
}

TrustTransaction::~TrustTransaction()
{
    delete ui;
}

/*
 * Close window
 */
void TrustTransaction::on_pushButton_cancel_clicked()
{
    TrustTransaction::close();
}

/*
 * Setup deposit
 */
void TrustTransaction::setupDeposit()
{
    isDeposit = true;
    TrustTransaction::setWindowTitle("Deposit");
    ui->pushButton_save->setText("Deposit funds");
}

/*
 * Setup withdrawal
 */
void TrustTransaction::setupWithdrawal()
{
    isDeposit = false;
    TrustTransaction::setWindowTitle("Withdrawal");
    ui->pushButton_save->setText("Withdrawal from trust");
}

/*
 * Save transaction
 */
void TrustTransaction::on_pushButton_save_clicked()
{
    QString client = ui->comboBox_client->currentText();
    QString paytype = ui->comboBox_payMethod->currentText();
    int amount = ui->lineEdit_tAmount->text().toInt();

    QSqlQuery query(db.db);

    if(!client.isEmpty() && !paytype.isEmpty() && amount > 0) {

        if(isDeposit) {
            //deposit

            query.prepare(QString("INSERT INTO trust_transactions (type, clientname, date, amount, created_by, pay_method) "
                                  "VALUES(:type, :client, :date, :amt, :by, :paytype) "));

            query.bindValue(":type", "Deposit");
            query.bindValue(":client", client);
            query.bindValue(":date", QDate::currentDate());
            query.bindValue(":amt", amount);
            query.bindValue(":by", db.getCurrentUserFullName());
            query.bindValue(":paytype", paytype);

            if(query.exec()){

                QMessageBox::information(this, "Funds deposited!",
                                         "Deposit saved successfully!", QMessageBox::Close);
            } else {
                qDebug() << query.lastError().text();
            }
        } else {
            //withdrawal

            query.prepare(QString("INSERT INTO trust_transactions (type, clientname, date, amount, created_by, pay_method) "
                                  "VALUES(:type, :client, :date, :amt, :by, :paytype) "));

            query.bindValue(":type", "Withdrawal");
            query.bindValue(":client", client);
            query.bindValue(":date", QDate::currentDate());
            query.bindValue(":amt", amount);
            query.bindValue(":by", db.getCurrentUserFullName());
            query.bindValue(":paytype", paytype);

            if(query.exec()){

                QMessageBox::information(this, "Funds withdrawn!",
                                         "Withdrawal saved successfully!", QMessageBox::Close);
            } else {
                qDebug() << query.lastError().text();
            }

        }

    }else{

        QMessageBox::warning(this, "Sorry!",
                             "Some fields have not been filled in.", QMessageBox::Close);
    }
}

/*
 * Get client balance function
 */
void TrustTransaction::getClientBalance()
{
    QString client = ui->comboBox_client->currentText();
    QSqlQuery query(db.db);
    //deposit amount
    int deposits = 0;
    query.exec("SELECT SUM(amount) FROM trust_transactions WHERE clientname = '"%client%"' AND type = 'Deposit' "
                                                                                        "AND archived = '0' ");
    while (query.next()) {
        deposits = query.value(0).toInt();
    }
    //withdrawal amount
    int withdrawals = 0;
    query.exec("SELECT SUM(amount) FROM trust_transactions WHERE clientname = '"%client%"' AND type = 'Withdrawal' "
                                                                                        "AND archived = '0' ");
    while (query.next()) {
        withdrawals = query.value(0).toInt();
    }
    //balance
    int balance = deposits - withdrawals;
    ui->lineEdit_balance->setText(QString::number(balance));
}

/*
 * Get client balance
 */
void TrustTransaction::on_comboBox_client_activated(const QString &arg1)
{
    getClientBalance();
}

void TrustTransaction::on_comboBox_client_currentIndexChanged(const QString &arg1)
{
    getClientBalance();
}

void TrustTransaction::on_comboBox_client_currentTextChanged(const QString &arg1)
{
    getClientBalance();
}

/*
 * Balance after transaction
 */
void TrustTransaction::on_lineEdit_tAmount_editingFinished()
{
    if(isDeposit){
        int balance = ui->lineEdit_balance->text().toInt() + ui->lineEdit_tAmount->text().toInt();
        ui->lineEdit_balAfterTransaction->setText(QString::number(balance));
    }else{
        int balance = ui->lineEdit_balance->text().toInt() - ui->lineEdit_tAmount->text().toInt();
        ui->lineEdit_balAfterTransaction->setText(QString::number(balance));
    }
}
