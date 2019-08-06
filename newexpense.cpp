#include "newexpense.h"
#include "ui_newexpense.h"

NewExpense::NewExpense(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewExpense)
{
    ui->setupUi(this);

    db.db = QSqlDatabase::database("bill");

    //expense type
    QSqlQueryModel *expTypeModel = new QSqlQueryModel();
    expTypeModel->setQuery("SELECT DISTINCT type FROM expense_types", db.db);
    ui->comboBox_expType->setModel(expTypeModel);

    //sender
    ui->lineEdit_sender->setText(db.getCurrentUserFullName());

    //receiver
    QSqlQueryModel *usersModel = new QSqlQueryModel();
    usersModel->setQuery("SELECT DISTINCT fullname FROM users", db.db);
    ui->comboBox_receiver->setModel(usersModel);

    userCompleter = new QCompleter(usersModel, this);
    userCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->comboBox_receiver->setCompleter(userCompleter);

    //clients
    QSqlQueryModel *clientsModel = new QSqlQueryModel();
    clientsModel->setQuery("SELECT DISTINCT clientname FROM clients", db.db);
    ui->comboBox_client->setModel(clientsModel);

    clientCompleter = new QCompleter(clientsModel, this);
    clientCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->comboBox_client->setCompleter(clientCompleter);

    //cases
    casesModel = new QSqlQueryModel();
    casesModel->setQuery("SELECT DISTINCT casename FROM cases", db.db);
    ui->comboBox_case->setModel(casesModel);

    caseCompleter = new QCompleter(casesModel, this);
    caseCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->comboBox_case->setCompleter(caseCompleter);
}

NewExpense::~NewExpense()
{
    delete ui;
}

/*
 * Close window
 */
void NewExpense::on_pushButton_cancel_clicked()
{
    NewExpense::close();
}

/*
 * Submit expense request
 */
void NewExpense::on_pushButton_submit_clicked()
{
    QString expType = ui->comboBox_expType->currentText();
    QString sender = db.getCurrentUserFullName();
    QString receiver = ui->comboBox_receiver->currentText();
    double amount = ui->lineEdit_amount->text().toDouble();
    QString date = QDate::currentDate().toString("yyyy/MM/dd");
    QString client = ui->comboBox_client->currentText();
    QString casename = ui->comboBox_case->currentText();
    QString description = ui->plainTextEdit_desc->toPlainText();

    if(!expType.isEmpty() && !sender.isEmpty() && !receiver.isEmpty() && amount > 0){
        QSqlQuery query(db.db);

            query.prepare(QString("INSERT INTO expenses (expense_type, description, amount, created_by, date_created, clientname,"
                                  " casename, receiver) VALUES(:type, :desc, :amt, :by, :date, :client, :case, :rec) "));

            query.bindValue(":type", expType);
            query.bindValue(":desc", description);
            query.bindValue(":amt", amount);
            query.bindValue(":by", sender);
            query.bindValue(":date", date);
            query.bindValue(":client", client);
            query.bindValue(":case", casename);
            query.bindValue(":rec", receiver);

            if(query.exec()){
                QMessageBox::information(this, "Success!",
                                         "Expense request submitted successfully!", QMessageBox::Close);
            } else {
                qDebug() << query.lastError().text();

                QMessageBox::warning(this, "Error!",
                                     "Failed to submit request!", QMessageBox::Close);
            }
    } else {
        QMessageBox::information(this, "Blank fields!", "Pleae fill in required fields!\n"
                                                        "(Expense type, sender, receiver and amount)",
                                 QMessageBox::Close);
    }
}

/*
 * Load client cases
 */
void NewExpense::on_comboBox_client_activated(const QString &arg1)
{
    casesModel->setQuery("SELECT DISTINCT casename FROM cases WHERE clientname ='"%arg1%"'", db.db);
    ui->comboBox_case->setModel(casesModel);
}
