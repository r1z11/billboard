#include "newclient.h"
#include "ui_newclient.h"

newclient::newclient(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::newclient)
{
    ui->setupUi(this);

    db.db = QSqlDatabase::database("bill");

    QStringList businessTypes;
    businessTypes << "Personal or Individual" << "Partnership" << "Company" << "NGO" << "International organisation" << "Other";
    ui->comboBox_businessTypes->addItems(businessTypes);

    QStringList billTypes;
    billTypes << "Hourly" << "Retainer" << "Both";
    ui->comboBox_billTypes->addItems(billTypes);
}

newclient::~newclient()
{
    delete ui;
}

//Close new client
void newclient::on_pushButton_cancel_clicked()
{
    newclient::close();
}

//Add new client
void newclient::on_pushButton_addClient_clicked()
{
    today = QDateTime::currentDateTime();

    QString cname = ui->lineEdit_clientName->text();
    QString email = ui->lineEdit_email->text();
    QString phone = ui->lineEdit_phNo->text();
    QString address = ui->plainTextEdit_address->toPlainText();
    QString pobox = ui->lineEdit_pobox->text();
    QString city = ui->lineEdit_city->text();
    QString state = ui->lineEdit_state->text();
    QString country = ui->lineEdit_country->text();

    QString dateCreated = today.date().toString("yyyy/MM/dd");
    QString lastModified = today.toString("yyyy/MM/dd hh:mm:ss");

    QString createdBy = db.getCurrentUserFullName();

    QDate dob;
    QString occupation;
    if(ui->dateEdit_dob->isEnabled()) {
        dob = ui->dateEdit_dob->date();
        occupation = ui->lineEdit_occupation->text();
    } else {
        //dob.setDate(NULL, NULL, NULL);
        occupation = "";
    }
    QString businessType = ui->comboBox_businessTypes->currentText();
    QString billType = ui->comboBox_billTypes->currentText();
    QString idPath = ui->lineEdit_idPath->text();
    QString referral = ui->lineEdit_referral->text();
    QString status = "Active";

    QString pname = ui->lineEdit_pName->text();
    QString pnumber = ui->lineEdit_pPhone->text();
    QString pdesignation = ui->lineEdit_designation->text();
    QString pemail = ui->lineEdit_pEmail->text();

    //insert data into database
    if(cname != NULL && email != NULL && phone != NULL && address != NULL){

        QSqlQuery query(db.db);

        query.prepare(QString("INSERT INTO clients (clientname, status, email, phoneno, date_created, created_by, address, "
                              "dob_or_startdt, occupation, business_type, billtype, id_scan, referral, pobox, city, state, country, last_modified) VALUES (:clientname, :status, "
                              ":email, :phone, :date_created, :created_by, :address, :dob, :occupation, :businessType, :billType, :idPath, "
                              ":referral, :pobox, :city, :state, :country, :lastModified) "));

        query.bindValue(":clientname", cname);
        query.bindValue(":status", status);
        query.bindValue(":email", email);
        query.bindValue(":phone", phone);
        query.bindValue(":date_created", dateCreated);
        query.bindValue(":created_by", createdBy);
        query.bindValue(":address", address);
        query.bindValue(":dob", dob);
        query.bindValue(":occupation", occupation);
        query.bindValue(":businessType", businessType);
        query.bindValue(":billType", billType);
        query.bindValue(":idPath", idPath);
        query.bindValue(":referral", referral);
        query.bindValue(":pobox", pobox);
        query.bindValue(":city", city);
        query.bindValue(":state", state);
        query.bindValue(":country", country);
        query.bindValue(":lastModified", lastModified);

        if(query.exec()){
            QMessageBox::information(this, "Success!",
                                     "New client added successfully!", QMessageBox::Close);
        }else{
            qDebug() << query.lastError().text();

            QMessageBox::warning(this, "Error!",
                                 "Failed to save the new client!", QMessageBox::Close);
        }
    }else{

        QMessageBox::warning(this, "Sorry!",
                             "Some fields have not been filled in.", QMessageBox::Close);
    }
}

//Company or organisation checked
void newclient::on_checkBox_co_clicked(bool checked)
{
    if(checked){
        ui->dateEdit_dob->setEnabled(false);
        ui->lineEdit_occupation->setEnabled(false);
    } else {
        ui->dateEdit_dob->setEnabled(true);
        ui->lineEdit_occupation->setEnabled(true);
    }
}

//Browse image
void newclient::on_pushButton_browse_clicked()
{
    //get the filename and directory to save exported data
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFiles);

    QString idpath = QFileDialog::getOpenFileName(
                this,
                "Select ID image",
                "C:\\Users",
                "Images (*.jpg *.png *.bmp)");

    ui->lineEdit_idPath->setText(idpath);
    ui->frame_ID->setStyleSheet("image: url("+idpath+");"
                                                         "background-color: white;");
}

/*
 * Same info as client toggled
 */
void newclient::on_checkBox_sameInfoAsClient_toggled(bool checked)
{
    if(checked) {
        ui->lineEdit_pName->setDisabled(true);
        ui->lineEdit_designation->setDisabled(true);
        ui->lineEdit_pPhone->setDisabled(true);
        ui->lineEdit_pEmail->setDisabled(true);
    } else {
        ui->lineEdit_pName->setDisabled(false);
        ui->lineEdit_designation->setDisabled(false);
        ui->lineEdit_pPhone->setDisabled(false);
        ui->lineEdit_pEmail->setDisabled(false);
    }
}
