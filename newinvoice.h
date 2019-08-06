#ifndef NEWINVOICE_H
#define NEWINVOICE_H

#include <QDialog>
#include <QCompleter>
#include <QFileDialog>
#include <QStandardItemModel>

#include <dbfunctions.h>

namespace Ui {
class NewInvoice;
}

class NewInvoice : public QDialog
{
    Q_OBJECT

public:
    explicit NewInvoice(QWidget *parent = 0);
    ~NewInvoice();

private slots:
    void on_lineEdit_search_returnPressed();

    void on_pushButton_search_clicked();

    void on_tableView_clients_clicked(const QModelIndex &index);

    void on_comboBox_cases_currentIndexChanged(const QString &arg1);

    void on_comboBox_cases_currentTextChanged(const QString &arg1);

    void on_pushButton_refresh_clicked();

    void on_pushButton_add_clicked();

    void on_pushButton_remove_clicked();

    void on_pushButton_printPrev_clicked();

    void on_pushButton_print_clicked();

    void on_pushButton_cancel_clicked();

    void on_pushButton_clear_clicked();

    void on_pushButton_draft_clicked();

    void on_pushButton_send_clicked();

private:
    Ui::NewInvoice *ui;

    DBfunctions db;
    QCompleter *csearchCompleter;
    QStandardItemModel *amodel;
    double nhrs;

    void setupClientsTable();
    void searchClients(QString clientname);
    void calculateBillDetails(QString casename);
    bool saveInvoice();
    bool sendInvoice();
};

#endif // NEWINVOICE_H
