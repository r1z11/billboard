#include "newcustodyitem.h"
#include "ui_newcustodyitem.h"

NewCustodyItem::NewCustodyItem(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewCustodyItem)
{
    ui->setupUi(this);

    db.db = QSqlDatabase::database("bill");

    db.clients = new QSqlTableModel(this, db.db);
    db.clients->setTable("clients");
    db.clients->setFilter("archived = '0' ");
    db.clients->select();
    int clientidCol = db.clients->fieldIndex("clientid");
    db.clients->setSort(clientidCol, Qt::AscendingOrder);
    db.clients->setEditStrategy(QSqlTableModel::OnFieldChange);

    //add auto complete to search clients
    QSqlQueryModel *clientsModel = new QSqlQueryModel();
    clientsModel->setQuery("SELECT DISTINCT clientname FROM clients", db.db);
//    csearchCompleter = new QCompleter(clientsModel, this);
//    csearchCompleter->setCompletionColumn(0);
//    csearchCompleter->setCaseSensitivity(Qt::CaseInsensitive);
//    ui->lineEdit_searchClients->setCompleter(csearchCompleter);
}

NewCustodyItem::~NewCustodyItem()
{
    delete ui;
}

/*
 * Close window
 */
void NewCustodyItem::on_pushButton_close_clicked()
{
    NewCustodyItem::close();
}
