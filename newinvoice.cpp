#include "newinvoice.h"
#include "ui_newinvoice.h"

NewInvoice::NewInvoice(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewInvoice)
{
    ui->setupUi(this);

    db.db = QSqlDatabase::database("bill");

    //clients
    db.iClients = new QSqlTableModel(this, db.db);
    db.iClients->setTable("clients");
    db.iClients->setFilter("archived = '0' ");
    db.iClients->select();
    setupClientsTable();

    QSqlQueryModel *cmodel = new QSqlQueryModel();
    cmodel->setQuery("SELECT DISTINCT clientname FROM clients", db.db);
    csearchCompleter = new QCompleter(cmodel, this);
    csearchCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->lineEdit_search->setCompleter(csearchCompleter);

    setupClientsTable();

    //billing type
    QStringList billTypes;
    billTypes << "Hourly" << "Retainer" << "Both";
    ui->comboBox_billType->addItems(billTypes);

    //attachments model
    amodel = new QStandardItemModel(this);
    amodel->setHeaderData(0, Qt::Horizontal, tr("File name"));
    ui->tableView_attachments->setModel(amodel);

    //status
    QStringList statuses;
    statuses << "Not sent" << "Sent" << "Paid" << "Overdue";
    ui->comboBox_status->addItems(statuses);
}

NewInvoice::~NewInvoice()
{
    delete ui;
}

/*
Setup clients table
*/
void NewInvoice::setupClientsTable()
{
    db.iClients->setHeaderData(1, Qt::Horizontal, tr("Client name"));

    ui->tableView_clients->setModel(db.iClients);

    ui->tableView_clients->hideColumn(0); // don't show the clientid
    ui->tableView_clients->hideColumn(2); // don't show the email
    ui->tableView_clients->hideColumn(3); // don't show the phone no.
    ui->tableView_clients->hideColumn(4); // don't show the address
    ui->tableView_clients->hideColumn(5); // don't show address 2
    ui->tableView_clients->hideColumn(6); // don't show the city
    ui->tableView_clients->hideColumn(7); // don't show the state
    ui->tableView_clients->hideColumn(8); // don't show the po box
    ui->tableView_clients->hideColumn(9); // don't show the status
    ui->tableView_clients->hideColumn(10); // don't show status
    ui->tableView_clients->hideColumn(11); // don't show date_created
    ui->tableView_clients->hideColumn(12); // don't show created_by
    ui->tableView_clients->hideColumn(13); // don't show last_modified
    ui->tableView_clients->hideColumn(14); // don't show modified_by
    ui->tableView_clients->hideColumn(15); // don't show the dob
    ui->tableView_clients->hideColumn(16); // don't show the id scan path
    ui->tableView_clients->hideColumn(17); // don't show the referral
    ui->tableView_clients->hideColumn(18); // don't show the occupation
    ui->tableView_clients->hideColumn(19); // don't show the bill type
    ui->tableView_clients->hideColumn(20); // don't show the payment method
    ui->tableView_clients->hideColumn(21); // don't show the business type
    ui->tableView_clients->hideColumn(22); // don't show the pname
    ui->tableView_clients->hideColumn(23); // don't show the pdesignation
    ui->tableView_clients->hideColumn(24); // don't show the pnumber
    ui->tableView_clients->hideColumn(25); // don't show the pemail
    ui->tableView_clients->hideColumn(26); // don't show archived

    ui->tableView_clients->setColumnWidth(1, 150);
    ui->tableView_clients->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableView_clients->horizontalHeader()->show();
    ui->tableView_clients->verticalHeader()->show();
    ui->tableView_clients->show();
}

/*
 * Search clients on return pressed
 */
void NewInvoice::on_lineEdit_search_returnPressed()
{
    QString client = ui->lineEdit_search->text();
    searchClients(client);
}

/*
 * Search clients on search button pressed
 */
void NewInvoice::on_pushButton_search_clicked()
{
    QString client = ui->lineEdit_search->text();
    searchClients(client);
}

/*
 * Search clients function
 */
void NewInvoice::searchClients(QString clientname)
{
    db.iClients->setTable("clients");
    db.iClients->setFilter("clientname LIKE %"%clientname%"% AND archived = '0' ");
    db.iClients->select();
    setupClientsTable();
}

/*
 * Get client info
 */
void NewInvoice::on_tableView_clients_clicked(const QModelIndex &index)
{
    //sender
    ui->lineEdit_sender->setText(db.getCurrentUserFullName());
    //client
    QString client = db.iClients->data(index).toString();
    ui->lineEdit_client->setText(client);
    //cases
    QSqlQueryModel *cmodel = new QSqlQueryModel();
    cmodel->setQuery("SELECT DISTINCT casename FROM cases WHERE clientname = '"%client%"'", db.db);
    ui->comboBox_cases->setModel(cmodel);
    //contact details
    ui->lineEdit_phone->setText(db.iClients->index(index.row(), 3).data().toString());
    ui->lineEdit_email->setText(db.iClients->index(index.row(), 2).data().toString());
    ui->lineEdit_poBox->setText(db.iClients->index(index.row(), 8).data().toString());
    ui->plainTextEdit_address->setPlainText(db.iClients->index(index.row(), 4).data().toString());
}

/*
 * Bill details on index changed
 */
void NewInvoice::on_comboBox_cases_currentIndexChanged(const QString &arg1)
{
    calculateBillDetails(arg1);
}

/*
 * Bill details on text changed
 */
void NewInvoice::on_comboBox_cases_currentTextChanged(const QString &arg1)
{
    calculateBillDetails(arg1);
}

/*
 * Calculate bill details function
 */
void NewInvoice::calculateBillDetails(QString casename)
{
    QSqlQuery query(db.db);
    //expenses
    double expenses;
    query.exec("SELECT SUM(amount) FROM expenses WHERE casename = '"%casename%"'");

    while (query.next()) {
        expenses = query.value(0).toDouble();
    }
    ui->lineEdit_expenses->setText(QString::number(expenses));

    //billable hours
    int caseid;
    query.exec("SELECT caseid FROM cases WHERE casename = '"%casename%"'");

    while (query.next()) {
        caseid = query.value(0).toInt();
    }
    double billhrs;
    query.exec("SELECT SUM(billhrs) FROM time_entries WHERE caseid = '"%QString::number(caseid)%"'");

    while (query.next()) {
        billhrs = query.value(0).toDouble();
    }
    ui->lineEdit_hrs->setText(QString::number(billhrs));

    //professional fee
    double fee;
    query.exec("SELECT SUM(amount) FROM time_entries WHERE caseid = '"%QString::number(caseid)%"'");

    while (query.next()) {
        fee = query.value(0).toDouble();
    }
    ui->lineEdit_proFee->setText(QString::number(fee));

    //non billable hours
    query.exec("SELECT SUM(nonbillhrs) FROM time_entries WHERE caseid = '"%QString::number(caseid)%"'");

    while (query.next()) {
        nhrs = query.value(0).toDouble();
    }

    //VAT
    double vat = .18 * fee;
    ui->lineEdit_vat->setText(QString::number(vat));

    //total
    double total = vat + fee + expenses;
    ui->lineEdit_total->setText(QString::number(total));
}

/*
 * Refresh client list
 */
void NewInvoice::on_pushButton_refresh_clicked()
{
    db.iClients->setTable("clients");
    db.iClients->setFilter("archived = '0' ");
    db.iClients->select();
    setupClientsTable();
}

/*
 * Add attachment
 */
void NewInvoice::on_pushButton_add_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFiles);

    QStringList apaths = QFileDialog::getOpenFileNames(
                this,
                "Select attachment",
                "C:\\Users",
                "All files (*)");

    for(int i=0; i<apaths.length(); i++){
        amodel->appendRow(new QStandardItem(apaths.at(i)));
    }
}

//Remove selected item from attachments
void NewInvoice::on_pushButton_remove_clicked()
{
    QModelIndexList indexes = ui->tableView_attachments->selectionModel()->selectedRows();

    while (!indexes.isEmpty())
    {
        amodel->removeRows(indexes.last().row(), 1);
        indexes.removeLast();
    }
}

/*
 * Print preview
 */
void NewInvoice::on_pushButton_printPrev_clicked()
{
    //print preview
}

/*
 * Print
 */
void NewInvoice::on_pushButton_print_clicked()
{
    //print
}

/*
 * Close window
 */
void NewInvoice::on_pushButton_cancel_clicked()
{
    NewInvoice::close();
}

/*
 * Clear fields
 */
void NewInvoice::on_pushButton_clear_clicked()
{
    //sender
    ui->lineEdit_sender->setText("");
    //client
    ui->lineEdit_client->setText("");
    //cases
    QSqlQueryModel *cmodel = new QSqlQueryModel();
    cmodel->setQuery("SELECT DISTINCT casename FROM cases", db.db);
    ui->comboBox_cases->setModel(cmodel);
    //bill details
    ui->lineEdit_expenses->setText("");
    ui->lineEdit_hrs->setText("");
    ui->lineEdit_proFee->setText("");
    //contact details
    ui->lineEdit_phone->setText("");
    ui->lineEdit_email->setText("");
    ui->lineEdit_poBox->setText("");
    ui->plainTextEdit_address->setPlainText("");
    //clear attachments
}

/*
 * Save invoice
 */
bool NewInvoice::saveInvoice()
{
    QString invno = ui->lineEdit_invNo->text();
    QString sender = ui->lineEdit_sender->text();
    QString client = ui->lineEdit_client->text();
    QString billtype = ui->comboBox_billType->currentText();
    QString casename = ui->comboBox_cases->currentText();
    QString status = ui->comboBox_status->currentText();
    double exp = ui->lineEdit_expenses->text().toDouble();
    double hrs = ui->lineEdit_hrs->text().toDouble();
    double fee = ui->lineEdit_proFee->text().toDouble();
    double vat = ui->lineEdit_vat->text().toDouble();
    double total = ui->lineEdit_total->text().toDouble();
//    QString phone = ui->lineEdit_phone->text();
    QString email = ui->lineEdit_email->text();
//    QString pobox = ui->lineEdit_poBox->text();
//    QString address = ui->plainTextEdit_address->toPlainText();
    QString notes = ui->plainTextEdit_notes->toPlainText();

    if(invno != NULL && sender != NULL && client != NULL && email != NULL) {

        QSqlQuery query(db.db);

        query.prepare(QString("INSERT INTO invoices (invoice_no, clientname, casename, vat, notes, status, created_by"
                              ", date_created, billable_hrs, nonbillable_hrs, bill_type, service_amt, expense_amt, amount) VALUES(:invno, :client, "
                              ":casename, :vat, :notes, :status, :created_by, :date_created, :billable_hrs, :nonbillable_hrs, :billType, "
                              ":service_amt, :expense_amt, :total) "));

        query.bindValue(":invno", invno);
        query.bindValue(":client", client);
        query.bindValue(":casename", casename);
        query.bindValue(":vat", vat);
        query.bindValue(":notes", notes);
        query.bindValue(":status", status);
        query.bindValue(":created_by", sender);
        query.bindValue(":date_created", QDate::currentDate());
        query.bindValue(":billable_hrs", hrs);
        query.bindValue(":nonbillable_hrs", nhrs);
        query.bindValue(":billType", billtype);
        query.bindValue(":service_amt", fee);
        query.bindValue(":expense_amt", exp);
        query.bindValue(":total", total);

        if(query.exec()){
            return true;
        } else {
            qDebug() << query.lastError().text();
        }

    }else{

        QMessageBox::warning(this, "Sorry!",
                             "Some fields have not been filled in.", QMessageBox::Close);
    }
    return false;
}

/*
 * Save draft
 */
void NewInvoice::on_pushButton_draft_clicked()
{
    ui->comboBox_status->setCurrentText("Not sent");
    if(saveInvoice()){
        QMessageBox::information(this, "Draft saved",
                                 "The draft invoice was successfully saved!", QMessageBox::Close);
    } else {
        QMessageBox::warning(this, "Sorry!",
                             "Failed to save draft.", QMessageBox::Close);
    }
}

/*
 * Send invoice
 */
void NewInvoice::on_pushButton_send_clicked()
{
    QString client = ui->lineEdit_client->text();
    ui->comboBox_status->setCurrentText("Sent");

    QMessageBox::StandardButton reply;

    reply = QMessageBox::question(this, "Send invoice",
                                  "Are you sure you want to send this invoice to\n"%
                                  client%" ?",
                                  QMessageBox::Yes | QMessageBox::No);

    if(reply == QMessageBox::Yes){
        if(sendInvoice()){
            saveInvoice();
            QMessageBox::information(this, "Success!",
                                     "The invoice was sent successfully to "%client, QMessageBox::Close);
        } else {
            QMessageBox::warning(this, "Error!",
                                     "Failed to send the invoice to "%client, QMessageBox::Close);
        }
    }
}

/*
 * Send invoice function
 */
bool NewInvoice::sendInvoice()
{
    return false;
}
