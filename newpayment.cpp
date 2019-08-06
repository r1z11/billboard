#include "newpayment.h"
#include "ui_newpayment.h"

NewPayment::NewPayment(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewPayment)
{
    ui->setupUi(this);

    db.db = QSqlDatabase::database("bill");

    //clients
    QSqlQueryModel *cmodel = new QSqlQueryModel();
    cmodel->setQuery("SELECT DISTINCT clientname FROM clients WHERE archived = '0' ", db.db);
    ui->comboBox_client->setModel(cmodel);

    //payment types
    QStringList payTypes;
    payTypes << "Cash" << "Cheque" << "Electronic transfer";
    ui->comboBox_payType->addItems(payTypes);
}

NewPayment::~NewPayment()
{
    delete ui;
}

/*
 * Close window
 */
void NewPayment::on_pushButton_cancel_clicked()
{
    NewPayment::close();
}

/*
 * Get client cases
 */
void NewPayment::on_comboBox_client_activated(const QString &arg1)
{
    getClientCases(arg1);
}

/*
 * Get client cases function
 */
void NewPayment::getClientCases(QString clientname)
{
    QSqlQueryModel *cmodel = new QSqlQueryModel();
    cmodel->setQuery("SELECT DISTINCT casename FROM cases WHERE clientname = '"%clientname%"' AND archived = '0' ", db.db);
    ui->comboBox_case->setModel(cmodel);
}

/*
 * Get case invoice numbers
 */
void NewPayment::on_comboBox_case_activated(const QString &arg1)
{
    QSqlQueryModel *imodel = new QSqlQueryModel();
    imodel->setQuery("SELECT DISTINCT invoice_no FROM invoices WHERE casename = '"%arg1%"' AND archived = '0' ", db.db);
    ui->comboBox_invno->setModel(imodel);
}

/*
 * Get amount paid, total bill and balance
 */
void NewPayment::on_comboBox_invno_activated(const QString &arg1)
{
    //amount paid
    QSqlQuery query(db.db);
    double amtPaid;
    query.exec("SELECT SUM(amount) FROM invoice_payments WHERE invoice_no = '"%arg1%"' AND archived = '0' ");

    while (query.next()) {
        amtPaid = query.value(0).toDouble();
    }
    ui->lineEdit_amtPaid->setText(QString::number(amtPaid));

    //total bill
    double bill;
    query.exec("SELECT amount FROM invoices WHERE invoice_no = '"%arg1%"' AND archived = '0' ");

    while (query.next()) {
        bill = query.value(0).toDouble();
    }
    ui->lineEdit_bill->setText(QString::number(bill));

    //balance
    double balance = bill - amtPaid;
    ui->lineEdit_balance->setText(QString::number(balance));
}

/*
 * Pay invoice
 */
void NewPayment::on_pushButton_pay_clicked()
{
    QString client = ui->comboBox_client->currentText();
    QString casename = ui->comboBox_case->currentText();
    QString invno = ui->comboBox_invno->currentText();
    double amtPaid = ui->lineEdit_amtPaid->text().toDouble();
    //double bill = ui->lineEdit_bill->text().toDouble();
    //double balance = ui->lineEdit_balance->text().toDouble();
    QString payType = ui->comboBox_payType->currentText();
    QString notes = ui->plainTextEdit_notes->toPlainText();
    bool paidInFull = ui->checkBox_paidInFull->isChecked();

    if(invno != NULL && client != NULL) {

        QSqlQuery query(db.db);

        query.prepare(QString("INSERT INTO invoice_payments (invoice_no, date_paid, clientname, casename, created_by"
                              ", payment_type, amount, paid_in_full, notes) VALUES(:invno, :date, :client, :casename, :by, :payType, "
                              ":amtPaid, :full, :notes) "));

        query.bindValue(":invno", invno);
        query.bindValue(":date", QDate::currentDate());
        query.bindValue(":client", client);
        query.bindValue(":casename", casename);
        query.bindValue(":by", db.getCurrentUserFullName());
        query.bindValue(":payType", payType);
        query.bindValue(":amtPaid", amtPaid);
        query.bindValue(":full", paidInFull);
        query.bindValue(":notes", notes);

        if(query.exec()){
            QMessageBox::information(this, "Payment saved!",
                                 "The new payment has been saved successfully!", QMessageBox::Close);
        } else {
            qDebug() << query.lastError().text();
        }

    }else {

        QMessageBox::warning(this, "Sorry!",
                             "Some fields have not been filled in.", QMessageBox::Close);
    }
}

/*
 * Get client cases
 */
void NewPayment::on_comboBox_client_currentTextChanged(const QString &arg1)
{
    getClientCases(arg1);
}
