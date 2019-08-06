#include "newcase.h"
#include "ui_newcase.h"

newcase::newcase(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::newcase)
{
    ui->setupUi(this);

    db.db = QSqlDatabase::database("bill");

    //add auto complete to clients comboBox
    QSqlQueryModel *cmodel = new QSqlQueryModel();
    cmodel->setQuery("SELECT DISTINCT clientname FROM clients WHERE archived = '0' " , db.db);
    clientCompleter = new QCompleter(cmodel, this);
    clientCompleter->setCompletionColumn(0);
    clientCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->comboBox_clients->setModel(cmodel);
    ui->comboBox_clients->setCompleter(clientCompleter);

    //add auto complete to clients comboBox
    QSqlQueryModel *pamodel = new QSqlQueryModel();
    pamodel->setQuery("SELECT DISTINCT parea FROM practice_areas" , db.db);
    pAreaCompleter = new QCompleter(pamodel, this);
    pAreaCompleter->setCompletionColumn(0);
    pAreaCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->comboBox_pArea->setModel(pamodel);
    ui->comboBox_pArea->setCompleter(pAreaCompleter);
}

newcase::~newcase()
{
    delete ui;
}

/*
 * Close window
 */
void newcase::on_pushButton_cancel_clicked()
{
    newcase::close();
}

/*
 * Save new case
 */
void newcase::on_pushButton_save_clicked()
{
    QString client = ui->comboBox_clients->currentText();
    QString casename = ui->lineEdit_caseName->text();
    QString caseno = ui->lineEdit_caseNo->text();
    QString parea = ui->comboBox_pArea->currentText();
    QString description = ui->plainTextEdit_desc->toPlainText();
    QString today = QDate::currentDate().toString("yyyy/MM/dd");
    QString status = "Open";
    QString user = db.getCurrentUserFullName();

    if(client != NULL && casename != NULL && parea != NULL){

        QSqlQuery query(db.db);

        query.prepare(QString("INSERT INTO cases (caseno, casename, date_created, status, practice_area"
                              ", created_by, clientname, description) VALUES(:caseno, :casename, :date_created, :status, :practice_area,"
                              " :user, :clientname, :description) "));

        query.bindValue(":caseno", caseno);
        query.bindValue(":casename", casename);
        query.bindValue(":date_created", today);
        query.bindValue(":status", status);
        query.bindValue(":practice_area", parea);
        query.bindValue(":user", user);
        query.bindValue(":clientname", client);
        query.bindValue(":description", description);

        if(query.exec()){
            QMessageBox::information(this, "Success!",
                                     "New case saved successfully!", QMessageBox::Close);
        }else{
            qDebug() << query.lastError().text();
            QMessageBox::warning(this, "Error!",
                                     "Failed to save the new case!", QMessageBox::Close);
        }

    }else{

        QMessageBox::warning(this, "Sorry!",
                             "Some fields have not been filled in.", QMessageBox::Close);
    }
}
