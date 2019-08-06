#include "newtimeentry.h"
#include "ui_newtimeentry.h"

double hrs, mins, ttlMins, rate, fee, ttlHrs;
QString feeStr;

NewTimeEntry::NewTimeEntry(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewTimeEntry)
{
    ui->setupUi(this);

    //get database connection
    db.db = QSqlDatabase::database("bill");

    //get current user
    QString user = db.getCurrentUserFullName();
    ui->label_user->setText(user);

    //get current date
    QDate today = QDate::currentDate();
    ui->dateEdit_te->setDate(today);

    //set billable option to true
    ui->radioButton_billable->setChecked(true);

    //get client names
    QSqlQueryModel *clientsmodel = new QSqlQueryModel();
    clientsmodel->setQuery("SELECT DISTINCT clientname FROM clients", db.db);
    clientsmodel->sort(0, Qt::AscendingOrder);
    ui->comboBox_clients->setModel(clientsmodel);
    ui->comboBox_clients->setModelColumn(0);

    //add auto complete to the clients dropdown
    clientscompleter = new QCompleter(clientsmodel, this);
    clientscompleter->setCompletionColumn(0);
    clientscompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->comboBox_clients->setCompleter(clientscompleter);

    //get rates
    QSqlQueryModel *ratemodel = new QSqlQueryModel();
    ratemodel->setQuery("SELECT DISTINCT rate FROM rates", db.db);
    ratemodel->sort(0, Qt::AscendingOrder);
    ui->comboBox_rate->setModel(ratemodel);
    ui->comboBox_rate->setModelColumn(0);
}

NewTimeEntry::~NewTimeEntry()
{
    delete ui;
}

/*
Billable radioButton toggled
*/
void NewTimeEntry::on_radioButton_billable_toggled(bool checked)
{
    if(checked){
        ui->radioButton_nonBillable->setChecked(false);
    }else{
        ui->radioButton_nonBillable->setChecked(true);
    }
}

/*
Set times
*/
void NewTimeEntry::setTime(double hrs, double mins)
{
    //set hrs and mins
    QString h = QString::number(hrs);
    QString m = QString::number(mins);

    qDebug()<<"dash " % h % ":" % m;

    ui->lineEdit_hrs->setText(h);
    ui->lineEdit_mins->setText(m);
}

/*
NonBillable radioButton toggled
*/
void NewTimeEntry::on_radioButton_nonBillable_toggled(bool checked)
{
    if(checked){
        ui->radioButton_billable->setChecked(false);
    }else{
        ui->radioButton_billable->setChecked(true);
    }
}

/*
Case link toggled
*/
void NewTimeEntry::on_checkBox_caseLink_toggled(bool checked)
{
    if(checked){
        ui->comboBox_cases->setEnabled(false);
    }else{
        ui->comboBox_cases->setEnabled(true);
    }
}

/*
Calculate fee function
*/
void NewTimeEntry::calculateFee()
{
    hrs = ui->lineEdit_hrs->text().toDouble();
    mins = ui->lineEdit_mins->text().toDouble();
    ttlMins = (hrs * 60) + mins;
    rate = ui->comboBox_rate->currentText().toDouble();
    ttlHrs = ttlMins/60;
    fee = ttlHrs * rate;
    ui->lineEdit_ttlHrs->setText(QString::number(ttlHrs));
    feeStr = QString::number(fee);
    ui->lineEdit_fee->setText(feeStr);
    ui->lineEdit_ttlFee->setText(feeStr);
}

/*
On rate selection changed
*/
void NewTimeEntry::on_comboBox_rate_currentIndexChanged(const QString &arg1)
{
    calculateFee();
}

/*
Calculate fee button
*/
void NewTimeEntry::on_pushButton_calcFee_clicked()
{
    calculateFee();
}

/*
On hrs changed
*/
void NewTimeEntry::on_lineEdit_hrs_editingFinished()
{
    calculateFee();
}

/*
On minutes changed
*/
void NewTimeEntry::on_lineEdit_mins_editingFinished()
{
    calculateFee();
}

/*
Close dialog
*/
void NewTimeEntry::on_pushButton_cancel_clicked()
{
    NewTimeEntry::close();
}

/*
Save time entry
*/
void NewTimeEntry::on_pushButton_save_clicked()
{
    //calculate total fees
    calculateFee();

    //get current user's full name and userid
    QString user = ui->label_user->text();
    int userid = db.getUserID(user);

    //today's date
    QDate curDate = ui->dateEdit_te->date();

    //whether billable or non billable hours
    bool billable = ui->radioButton_billable->isChecked();
    double billhrs = 0, nonbillhrs = 0;

    if(billable)
        billhrs = ui->lineEdit_ttlHrs->text().toDouble();
    else
        nonbillhrs = ui->lineEdit_ttlHrs->text().toDouble();

    //get client's full name and id
    QString client = ui->comboBox_clients->currentText();
    int clientid = db.getClientID(client);

    //get case name and id
    bool caseLinked = ui->checkBox_caseLink->isChecked();
    QString casename;

    if(caseLinked)
        casename = ui->comboBox_cases->currentText();
    else
        casename = "";

    int caseid = db.getCaseID(casename);

    //get task name
    QString task = ui->comboBox_tasks->currentText();

    //description
    QString description = ui->plainTextEdit_desc->toPlainText();

    //insert data into database if client name, task name and hours have been entered
    if(client != NULL && task != NULL && hrs>0){

        QSqlQuery query(db.db);

        query.prepare(QString("INSERT INTO time_entries (userid, caseid, clientid, description, date_created"
                              ", created_by, modified_by, billhrs, nonbillhrs, rate, amount) VALUES(:userid, :caseid, :clientid, :description, :date"
                              ", :fullname, :fullname2, :billhrs, :nonbillhrs, :rate, :amount) "));

        query.bindValue(":userid", userid);
        query.bindValue(":caseid", caseid);
        query.bindValue(":clientid", clientid);
        query.bindValue(":description", description);
        query.bindValue(":date", curDate);
        query.bindValue(":fullname", user);
        query.bindValue(":fullname2", user);
        query.bindValue(":billhrs", billhrs);
        query.bindValue(":nonbillhrs", nonbillhrs);
        query.bindValue(":rate", rate);
        query.bindValue(":amount", fee);

        if(query.exec()){
            QMessageBox::information(this, "Success!",
                                     "Time entry saved successfully!", QMessageBox::Close);
        }else{
            QMessageBox::warning(this, "Error!",
                                     "Failed to save the new time entry!", QMessageBox::Close);
        }

    }else{

        QMessageBox::warning(this, "Sorry!",
                             "Some fields have not been filled in.", QMessageBox::Close);
    }
}

/*
 * Get client's cases
 */
void NewTimeEntry::on_comboBox_clients_activated(const QString &arg1)
{
    //get case names
    QSqlQueryModel *casemodel = new QSqlQueryModel();
    casemodel->setQuery("SELECT DISTINCT casename FROM cases WHERE clientname = '"%arg1%"'", db.db);
    casemodel->sort(0, Qt::AscendingOrder);
    ui->comboBox_cases->setModel(casemodel);
    ui->comboBox_cases->setModelColumn(0);

    //add auto complete to the cases dropdown
    casecompleter = new QCompleter(casemodel, this);
    casecompleter->setCompletionColumn(0);
    casecompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->comboBox_clients->setCompleter(casecompleter);
}

/*
 * Get case' tasks
 */
void NewTimeEntry::on_comboBox_cases_activated(const QString &arg1)
{
    //get task names
    int caseid;
    QSqlQuery query(db.db);
    query.exec("SELECT caseid FROM cases WHERE casename = '"%arg1%"'");

    while (query.next()) {
        caseid = query.value(0).toInt();
    }
    QSqlQueryModel *taskmodel = new QSqlQueryModel();
    taskmodel->setQuery("SELECT DISTINCT taskname FROM tasks WHERE caseid = '"%QString::number(caseid)%"'", db.db);
    taskmodel->sort(0, Qt::AscendingOrder);
    ui->comboBox_tasks->setModel(taskmodel);
    ui->comboBox_tasks->setModelColumn(0);

    //add auto complete to the tasks dropdown
    taskcompleter = new QCompleter(taskmodel, this);
    taskcompleter->setCompletionColumn(0);
    taskcompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->comboBox_tasks->setCompleter(taskcompleter);
}
