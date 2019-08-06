#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QMainWindow>
#include <QCompleter>
#include <QTableView>
#include <QMenu>
#include <QDebug>
#include <QFileSystemModel>

#include <dbfunctions.h>
#include <helperfuncs.h>
#include <newtimeentry.h>
#include <newtask.h>
#include <newclient.h>
#include <usertask.h>
#include <newcase.h>
#include <newexpense.h>
 #include <newinvoice.h>
#include <newpayment.h>
#include <trusttransaction.h>
#include <newcustodyitem.h>

namespace Ui {
class Dashboard;
}

class Dashboard : public QMainWindow
{
    Q_OBJECT

public:
    explicit Dashboard(QWidget *parent = 0);
    ~Dashboard();

    HelperFunctions hf;

private slots:
    void on_pushButton_startPause_clicked();

    void on_pushButton_stop_clicked();

    void updateTime();

    void on_pushButton_refreshHome_clicked();

    void on_pushButton_searchUTasks_clicked();

    void on_lineEdit_searchUTasks_returnPressed();

    void on_pushButton_newTime_clicked();

    void on_listWidget_menu_clicked(const QModelIndex &index);

    void on_pushButton_smTasks_clicked();

    void on_lineEdit_searchTasks_returnPressed();

    void on_pushButton_searchTasks_clicked();

    void on_pushButton_refreshTasks_clicked();

    void on_comboBox_stat_currentIndexChanged(const QString &arg1);

    void on_tableView_tasks_clicked(const QModelIndex &index);

    void on_pushButton_pending_clicked();

    void on_pushButton_finished_clicked();

    void on_comboBox_tCases_currentIndexChanged(const QString &arg1);

    void on_comboBox_priority_currentIndexChanged(const QString &arg1);

    void on_pushButton_newTask_clicked();

    void on_tableView_tasks_customContextMenuRequested(const QPoint &pos);

    void menuSelection(QAction* action);

    void on_pushButton_searchClients_clicked();

    void on_pushButton_newClient_clicked();

    void on_pushButton_refreshClients_clicked();

    void on_pushButton_deleteTask_clicked();

    void on_pushButton_markFinished_clicked();

    void on_pushButton_delUtask_clicked();

    void on_tableView_clients_clicked(const QModelIndex &index);

    void on_pushButton_myClients_clicked();

    void on_comboBox_users_activated(const QString &arg1);

    void on_pushButton_newTask_2_clicked();

    void on_comboBox_filterUtaskCase_activated(const QString &arg1);

    void on_comboBox_filterUtaskPriority_activated(const QString &arg1);

    void on_pushButton_openCases_clicked();

    void on_pushButton_closedCases_clicked();

    void on_pushButton_refreshCases_clicked();

    void on_comboBox_pAreas_activated(const QString &arg1);

    void on_pushButton_myCases_clicked();

    void on_lineEdit_searchCases_returnPressed();

    void on_pushButton_searchCases_clicked();

    void on_pushButton_delCase_clicked();

    void on_pushButton_delClient_clicked();

    void on_calendarWidget_clicked(const QDate &date);

    void on_comboBox_monthFilter_activated(int index);

    void on_listView_tasks_clicked(const QModelIndex &index);

    void on_pushButton_refreshCtasks_clicked();

    void on_pushButton_newCtask_clicked();

    void on_pushButton_delCtask_clicked();

    void on_pushButton_newCase_clicked();

    void on_treeView_clicked(const QModelIndex &index);

    void on_pushButton_newTimeEntry_clicked();

    void on_comboBox_teUsers_activated(const QString &arg1);

    void on_comboBox_teClients_activated(const QString &arg1);

    void on_comboBox_teCases_activated(const QString &arg1);

    void on_pushButton_refreshBilling_clicked();

    void on_pushButton_newExpense_clicked();

    void on_pushButton_filterUTByDate_clicked();

    void on_pushButton_filterTasksByDate_clicked();

    void on_tableView_docs_doubleClicked(const QModelIndex &index);

    void on_pushButton_teDateFilter_clicked();

    void on_comboBox_expUsers_activated(const QString &arg1);

    void on_comboBox_expType_activated(const QString &arg1);

    void on_comboBox_expClients_activated(const QString &arg1);

    void on_comboBox_expCases_activated(const QString &arg1);

    void on_pushButton_expDateFilter_clicked();

    void on_comboBox_inClients_activated(const QString &arg1);

    void on_comboBox_inCases_activated(const QString &arg1);

    void on_pushButton_newInvoice_clicked();

    void on_pushButton_invDateFilter_clicked();

    void on_pushButton_newPayment_clicked();

    void on_lineEdit_searchTF_returnPressed();

    void on_pushButton_searchTF_clicked();

    void on_pushButton_withdrawTF_clicked();

    void on_pushButton_depositTF_clicked();

    void on_pushButton_refreshTF_clicked();

    void on_pushButton_dateFilterTF_clicked();

    void on_tableView_trustFunds_clicked(const QModelIndex &index);

    void on_pushButton_ciRefresh_clicked();

    void on_pushButton_ciClientFilter_clicked();

    void on_pushButton_newCI_clicked();

    void on_pushButton_ciCheckout_clicked();

private:
    Ui::Dashboard *ui;

    DBfunctions db;
    QTimer *timer;
    QCompleter *utsearchcompleter, *tsearchcompleter, *tcompleter, *csearchCompleter, *casesearchCompleter,
    *usersCompleter;
    QFileSystemModel *fmodel;
    QSqlQueryModel *trustmodel;

    void setupTasksTable(QTableView *tasksTable, QSqlTableModel *tasks);
    void search_utasks(QString &taskname);
    void search_tasks(QString &taskname);
    void setupClientsTable(QTableView *clientsTable, QSqlTableModel *clients);
    void setupCasesTable(QTableView *casesTable, QSqlTableModel *cases);
    void searchClients(QString &clientname);
    void searchCases(QString &casename);
    bool isItemSelected(QTableView *tableView);
    void setupDocsTable(QTableView *docsTable, QFileSystemModel *files);
    void setupTimeEntriesTable();
    void refreshDash();
    void refreshTimeEntries();
    void refreshExpenses();
    void refreshInvoices();
    void setupExpensesTable();
    void setupInvoicesTable();
    void setupTrustFundsTable();
    void searchTrustFunds(QString clientname);
    void setupTrustTransactionsTable();
    void getClientTrustDetails(QString client);
    void setupCustodyItemsTable();
};

#endif // DASHBOARD_H
