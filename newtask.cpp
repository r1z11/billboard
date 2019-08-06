#include "newtask.h"
#include "ui_newtask.h"
#include <QSqlQueryModel>

newtask::newtask(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::newtask)
{
    ui->setupUi(this);

    //get database connection
    db.db = QSqlDatabase::database("bill");

    //get case names
    QSqlQueryModel *casemodel = new QSqlQueryModel();
    casemodel->setQuery("SELECT DISTINCT casename FROM cases", db.db);
    casemodel->sort(0, Qt::AscendingOrder);
    ui->comboBox_caseName->setModel(casemodel);
    ui->comboBox_caseName->setModelColumn(0);

    //add auto complete to the cases dropdown
    caseCompleter = new QCompleter(casemodel, this);
    caseCompleter->setCompletionColumn(0);
    caseCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->comboBox_caseName->setCompleter(caseCompleter);

    //get client names
    QSqlQueryModel *clientsmodel = new QSqlQueryModel();
    clientsmodel->setQuery("SELECT DISTINCT clientname FROM clients", db.db);
    clientsmodel->sort(0, Qt::AscendingOrder);
    ui->comboBox_clientName->setModel(clientsmodel);
    ui->comboBox_clientName->setModelColumn(0);

    //add auto complete to the clients dropdown
    clientCompleter = new QCompleter(clientsmodel, this);
    clientCompleter->setCompletionColumn(0);
    clientCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->comboBox_clientName->setCompleter(clientCompleter);

    //get current date
    QDate today = QDate::currentDate();
    ui->dateEdit_created->setDate(today);
    ui->dateEdit_due->setDate(today);
}

newtask::~newtask()
{
    delete ui;
}

void newtask::on_pushButton_addTask_clicked()
{
    //add task

    //get current user's full name and userid
    int caseid;
    if(!ui->checkBox_caseLink->isChecked()){
        QString caseName = ui->comboBox_caseName->currentText();
        caseid = db.getCaseID(caseName);
    }
    QString user = db.getCurrentUserFullName();

    //date created
    QDate dateCreated = ui->dateEdit_created->date();
    //due date
    QDate dueDate = ui->dateEdit_due->date();

    //get client's full name and id
    QString clientName = ui->comboBox_clientName->currentText();
    int clientid = db.getClientID(clientName);

    //get task name
    QString taskName = ui->lineEdit_taskName->text();

    //get priority
    QString priority = ui->comboBox_priority->currentText();

    //status
    QString status = "pending";

    //description
    QString description = ui->plainTextEdit_description->toPlainText();

    //insert data into database if client name, task name and hours have been entered
    if(clientName != NULL && taskName != NULL){

        QSqlQuery query(db.db);

        //if case link is checked, add caseid to insert statement
        if(!ui->checkBox_caseLink->isChecked()){
            query.prepare(QString("INSERT INTO tasks (caseid, clientid, taskname, priority, status, description, date_created"
                                  ", created_by, due_date, modified_by) VALUES(:caseid, :clientid, :taskname, :priority, :status, :description, :date_created"
                                  ", :created_by, :due_date, :modified_by) "));

            query.bindValue(":caseid", caseid);
            query.bindValue(":clientid", clientid);
            query.bindValue(":taskname", taskName);
            query.bindValue(":priority", priority);
            query.bindValue(":status", status);
            query.bindValue(":description", description);
            query.bindValue(":date_created", dateCreated);
            query.bindValue(":created_by", user);
            query.bindValue(":due_date", dueDate);
            query.bindValue(":modified_by", user);

            if(query.exec()){
                QMessageBox::information(this, "Success!",
                                         "New task saved successfully!", QMessageBox::Close);
            }else{
                qDebug() << query.lastError().text();

                QMessageBox::warning(this, "Error!",
                                     "Failed to save the new task!", QMessageBox::Close);
            }
        }else{
            query.prepare(QString("INSERT INTO tasks (clientid, taskname, priority, status, description, date_created"
                                  ", created_by, due_date, modified_by) VALUES(:clientid, :taskname, :priority, :status, :description, :date_created"
                                  ", :created_by, :due_date, :modified_by) "));

            query.bindValue(":clientid", clientid);
            query.bindValue(":taskname", taskName);
            query.bindValue(":priority", priority);
            query.bindValue(":status", status);
            query.bindValue(":description", description);
            query.bindValue(":date_created", dateCreated);
            query.bindValue(":created_by", user);
            query.bindValue(":due_date", dueDate);
            query.bindValue(":modified_by", user);

            if(query.exec()){
                QMessageBox::information(this, "Success!",
                                         "New task saved successfully!", QMessageBox::Close);
            }else{
                qDebug() << query.lastError().text();

                QMessageBox::warning(this, "Error!",
                                     "Failed to save the new task!", QMessageBox::Close);
            }
        }
    }else{

        QMessageBox::warning(this, "Sorry!",
                             "Some fields have not been filled in.", QMessageBox::Close);
    }
}

void newtask::on_pushButton_cancel_clicked()
{
    //close New task
    newtask::close();
}

void newtask::on_checkBox_caseLink_toggled(bool checked)
{
    if(checked){
        //disable cases comboBox
        ui->comboBox_caseName->setDisabled(true);
    }else{
        //enable cases comboBox
        ui->comboBox_caseName->setEnabled(true);
    }
}
