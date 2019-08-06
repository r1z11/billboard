#include "usertask.h"
#include "ui_usertask.h"

UserTask::UserTask(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserTask)
{
    ui->setupUi(this);

    //get database connection
    db.db = QSqlDatabase::database("bill");

    //get users' full names
    QSqlQueryModel *usermodel = new QSqlQueryModel();
    usermodel->setQuery("SELECT userid, fullname FROM users", db.db);
    usermodel->sort(0, Qt::AscendingOrder);
    ui->comboBox_users->setModel(usermodel);
    ui->comboBox_users->setModelColumn(1);

    //add auto complete to the user dropdown
    userCompleter = new QCompleter(usermodel, this);
    userCompleter->setCompletionColumn(0);
    userCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->comboBox_users->setCompleter(userCompleter);
}

UserTask::~UserTask()
{
    delete ui;
}

/*
Close window
*/
void UserTask::on_pushButton_cancel_clicked()
{
    UserTask::close();
}

/*
Set tasknames
*/
void UserTask::setTasknames(QStringList tasknames)
{
    tnames = tasknames;
    QString labelText;

    if(tasknames.isEmpty()){
        qDebug()<<"tasknames is NULL";
    }else{

        for(int i=0; i<tasknames.count(); i++){
            labelText.append(tasknames.at(i));
            if(i == tasknames.count()-1){
                labelText.append(".");
            }else{
                labelText.append(", ");
            }
        }

        ui->label_taskList->setText("Add user to task(s) "+labelText);
    }
}

/*
Add user to task
*/
void UserTask::on_pushButton_addUser_clicked()
{
    QString fullname = ui->comboBox_users->currentText();
    int userid = db.getUserID(fullname);

    QList<int> taskids;
    for(int i=0; i<tnames.count(); i++){
        taskids.append(db.getTaskID(tnames.at(i)));
    }

    QSqlQuery query(db.db);
    QString sqlQuery = "INSERT INTO user_tasks (userid, taskid) VALUES ";
    QString sqlQueryValues;

    if(taskids.isEmpty()){
        qDebug()<<"taskids is NULL";
    }else{

        int j;
        int count = taskids.count();
        QString tmpStr;

        for(int i=0; i<count; i++){
            j = taskids.at(i);

            tmpStr = "(" % QString::number(userid) % ", " % QString::number(j) % ") ";
            sqlQueryValues.append(tmpStr);

            if(i != (count-1)){
                sqlQueryValues.append(",");
            }
        }
        sqlQuery.append(sqlQueryValues);
        query.prepare(QString(sqlQuery));

        if(query.exec()){
            QMessageBox::information(0, qApp->tr("Saved!"),
                                     qApp->tr("User added successfully to task(s).\n"),
                                     QMessageBox::Close);
        }else{
            qDebug() << query.lastError().text();
            QMessageBox::critical(0, qApp->tr("An error occured!"),
                                  qApp->tr("Failed to add user to task(s).\n"),
                                  QMessageBox::Close);
        }
    }
}
