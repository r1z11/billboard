#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "dashboard.h"

LoginWindow::LoginWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);

    db.createConnection("bill");

    //set username if last user didn't logout
    QString user = db.getCurrentUser();

    if(user != NULL){
        ui->lineEdit_usrnm->setText(user);
    }

    //get user types
    db.utmodel = new QSqlTableModel(this, db.db);

    db.utmodel->setTable("usertypes");
    int col = db.utmodel->fieldIndex("usertype");
    db.utmodel->setSort(col, Qt::AscendingOrder);
    db.utmodel->select();

    ui->comboBox_usrtyp->setModel(db.utmodel);
    ui->comboBox_usrtyp->setModelColumn(col);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

/*
Login function
*/
void LoginWindow::login(){
    if(db.isDbOpen()){
        QString account = ui->comboBox_usrtyp->currentText();
        QString username = ui->lineEdit_usrnm->text();
        QString password = ui->lineEdit_pass->text();

        QSqlQuery query(db.db);

        query.prepare(QString("SELECT usertypes.usertype_id, usertypes.usertype, users.username, users.password "
                              "FROM usertypes INNER JOIN users ON usertypes.usertype_id=users.usertype_id WHERE "
                              "users.username = :username"));
        query.bindValue(":username", username);

        QString acc;
        QString user;
        QString pass;

        if(query.exec()) {

            while (query.next()) {
                acc = query.value(1).toString();
                user = query.value(2).toString();
                pass = query.value(3).toString();

                //check login details
                if(account == acc && username == user && password == pass){
                    //set current user
                    db.setCurrentUser(user);

                    //close login form
                    LoginWindow::hide();

                    //open dashboard
                    Dashboard *dash = new Dashboard();
                    dash->showMaximized();
                }else{
                    QMessageBox::warning(this, "Access Denied!",
                                         "Make sure you entered the right username and"
                                         " password along with the matching user type.", QMessageBox::Close);
                }
            }
        } else {
            qDebug() << query.lastError().text();
        }
    }
}

/*
Login on return pressed
*/
void LoginWindow::on_lineEdit_pass_returnPressed()
{
    login();
}

/*
Login button pressed
*/
void LoginWindow::on_pushButton_login_clicked()
{
    login();
}
