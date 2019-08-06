#include "dashboard.h"
#include "ui_dashboard.h"

bool start = true, pause = false;
bool timerStarted = false, timerStopped = false, timerPaused = false;
int secCounter = 0, selectedTaskRow;

Dashboard::Dashboard(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Dashboard)
{
    ui->setupUi(this);

    db.db = QSqlDatabase::database("bill");

    //set the home page
    ui->stackedWidget->setCurrentIndex(0);

    //setup the menu style sheet

    //UPCOMING TASKS
    db.utasks = new QSqlTableModel(this, db.db);
    db.utasks->setTable("tasks");
    db.utasks->setFilter("status = 'pending' AND archived = '0' ");
    db.utasks->select();
    int taskidCol = db.utasks->fieldIndex("taskid");
    db.utasks->setSort(taskidCol, Qt::AscendingOrder);
    db.utasks->setEditStrategy(QSqlTableModel::OnManualSubmit);
    setupTasksTable(ui->tableView_upcomingTasks, db.utasks);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));

    //add auto complete to search upcoming tasks
    QSqlQueryModel *utmodel = new QSqlQueryModel();
    utmodel->setQuery("SELECT DISTINCT taskname FROM tasks WHERE status = 'pending' && archived = '0' " , db.db);
    utsearchcompleter = new QCompleter(utmodel, this);
    utsearchcompleter->setCompletionColumn(0);
    utsearchcompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->lineEdit_searchUTasks->setCompleter(utsearchcompleter);

    //set tasks from and to dates
    QDate today = QDate::currentDate();
    ui->dateEdit_fromUtasks->setDate(today);
    ui->dateEdit_toUtasks->setDate(today);

    //get list of cases
    QSqlQueryModel *cmodel = new QSqlQueryModel();
    cmodel->setQuery("SELECT casename FROM cases", db.db);
    ui->comboBox_filterUtaskCase->setModel(cmodel);

    //add auto complete to filter by cases
    QSqlQueryModel *tcmodel = new QSqlQueryModel();
    tcmodel->setQuery("SELECT casename FROM cases", db.db);
    tcompleter = new QCompleter(tcmodel, this);
    tcompleter->setCompletionColumn(0);
    tcompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->comboBox_filterUtaskCase->setCompleter(tcompleter);

    //set task priorities
    QStringList priorities;
    priorities << "high" << "medium" << "low";
    ui->comboBox_filterUtaskPriority->addItems(priorities);

    //CALENDER
    db.ctasks = new QSqlTableModel(this, db.db);
    db.ctasks->setTable("tasks");
    db.ctasks->setFilter("archived = '0' ");
    db.ctasks->select();
    ui->listView_tasks->setModel(db.ctasks);
    ui->listView_tasks->setModelColumn(3);

    //TASKS
    db.tasks = new QSqlTableModel(this, db.db);
    db.tasks->setTable("tasks");
    db.tasks->select();
    int allTaskidsCol = db.utasks->fieldIndex("taskid");
    db.tasks->setSort(allTaskidsCol, Qt::AscendingOrder);
    db.tasks->setEditStrategy(QSqlTableModel::OnManualSubmit);
    setupTasksTable(ui->tableView_tasks, db.tasks);

    //add auto complete to search tasks
    QSqlQueryModel *tmodel = new QSqlQueryModel();
    tmodel->setQuery("SELECT DISTINCT taskname FROM tasks", db.db);
    tsearchcompleter = new QCompleter(tmodel, this);
    tsearchcompleter->setCompletionColumn(0);
    tsearchcompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->lineEdit_searchTasks->setCompleter(tsearchcompleter);

    //get list of users' full names
    QSqlQueryModel *statmodel = new QSqlQueryModel();
    statmodel->setQuery("SELECT fullname FROM users", db.db);
    ui->comboBox_stat->setModel(statmodel);
    //users combo box (clients tab)
    ui->comboBox_users->setModel(statmodel);

    //set tasks from and to dates
    ui->dateEdit_tFrom->setDate(today);
    ui->dateEdit_tTo->setDate(today);

    //get list of cases
    ui->comboBox_tCases->setModel(cmodel);

    //add auto complete to filter by cases
    ui->comboBox_tCases->setCompleter(tcompleter);

    //set task priorities
    ui->comboBox_priority->addItems(priorities);

    //refresh tasks
    on_pushButton_refreshTasks_clicked();

    //CLIENTS
    db.clients = new QSqlTableModel(this, db.db);
    db.clients->setTable("clients");
    db.clients->setFilter("archived = '0' ");
    db.clients->select();
    int clientidCol = db.clients->fieldIndex("clientid");
    db.clients->setSort(clientidCol, Qt::AscendingOrder);
    db.clients->setEditStrategy(QSqlTableModel::OnFieldChange);
    setupClientsTable(ui->tableView_clients, db.clients);

    //add auto complete to search clients
    QSqlQueryModel *clientsModel = new QSqlQueryModel();
    clientsModel->setQuery("SELECT DISTINCT clientname FROM clients", db.db);
    csearchCompleter = new QCompleter(clientsModel, this);
    csearchCompleter->setCompletionColumn(0);
    csearchCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->lineEdit_searchClients->setCompleter(csearchCompleter);

    //CASES
    db.cases = new QSqlTableModel(this, db.db);
    db.cases->setTable("cases");
    db.cases->setFilter("archived = '0' ");
    db.cases->select();
    int caseidCol = db.cases->fieldIndex("caseid");
    db.cases->setSort(caseidCol, Qt::AscendingOrder);
    db.cases->setEditStrategy(QSqlTableModel::OnFieldChange);
    setupCasesTable(ui->tableView_cases, db.cases);

    //add auto complete to search cases
    QSqlQueryModel *casesModel = new QSqlQueryModel();
    casesModel->setQuery("SELECT DISTINCT casename FROM cases", db.db);
    casesearchCompleter = new QCompleter(casesModel, this);
    casesearchCompleter->setCompletionColumn(0);
    casesearchCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->lineEdit_searchCases->setCompleter(casesearchCompleter);

    //get list of practice areas
    QSqlQueryModel *pamodel = new QSqlQueryModel();
    pamodel->setQuery("SELECT DISTINCT parea FROM practice_areas", db.db);
    ui->comboBox_pAreas->setModel(pamodel);

    //DOCUMENTS
    fmodel = new QFileSystemModel(this);
    fmodel->setRootPath("C:/");
    ui->treeView->setModel(fmodel);

    ui->treeView->setAnimated(true);
    ui->treeView->setIndentation(20);
    ui->treeView->setSortingEnabled(true);
    ui->treeView->setColumnWidth(0,250);
    ui->treeView->hideColumn(1);
    ui->treeView->hideColumn(2);
    ui->treeView->hideColumn(3);

    ui->tableView_docs->setModel(fmodel);

    setupDocsTable(ui->tableView_docs, fmodel);

    //TIME ENTRIES
    db.tentries = new QSqlTableModel(this, db.db);
    db.tentries->setTable("time_entries");
    db.tentries->setFilter("archived = '0' ");
    db.tentries->select();
    db.tentries->setEditStrategy(QSqlTableModel::OnFieldChange);
    setupTimeEntriesTable();

    //add auto complete to
    //users comboBox
    usersCompleter = new QCompleter(statmodel, this);
    usersCompleter->setCompletionColumn(0);
    usersCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->comboBox_teUsers->setModel(statmodel);
    ui->comboBox_teUsers->setCompleter(usersCompleter);
    //clients comboBox
    ui->comboBox_teClients->setModel(clientsModel);
    ui->comboBox_teClients->setCompleter(csearchCompleter);
    //cases comboBox
    ui->comboBox_teCases->setModel(casesModel);
    ui->comboBox_teCases->setCompleter(casesearchCompleter);

    ui->dateEdit_tEntries->setDate(today);

    //EXPENSES
    db.exp = new QSqlTableModel(this, db.db);
    db.exp->setTable("expenses");
    db.exp->setFilter("archived = '0' ");
    db.exp->select();
    db.exp->setEditStrategy(QSqlTableModel::OnManualSubmit);
    setupExpensesTable();

    //add auto complete to
    //users comboBox
    ui->comboBox_expUsers->setModel(statmodel);
    ui->comboBox_expUsers->setCompleter(usersCompleter);
    //clients comboBox
    ui->comboBox_expClients->setModel(clientsModel);
    ui->comboBox_expClients->setCompleter(csearchCompleter);
    //cases comboBox
    ui->comboBox_expCases->setModel(casesModel);
    ui->comboBox_expCases->setCompleter(casesearchCompleter);

    //expense type comboBox
    QSqlQueryModel *expTypeModel = new QSqlQueryModel();
    expTypeModel->setQuery("SELECT DISTINCT type FROM expense_types", db.db);
    ui->comboBox_expType->setModel(expTypeModel);

    ui->dateEdit_expenses->setDate(today);

    //INVOICES
    db.invoices = new QSqlTableModel(this, db.db);
    db.invoices->setTable("invoices");
    db.invoices->setFilter("archived = '0' ");
    db.invoices->select();
    db.invoices->setEditStrategy(QSqlTableModel::OnManualSubmit);
    setupInvoicesTable();

    //add auto complete to...
    //clients comboBox
    ui->comboBox_inClients->setModel(clientsModel);
    ui->comboBox_inClients->setCompleter(csearchCompleter);
    //cases comboBox
    ui->comboBox_inCases->setModel(casesModel);
    ui->comboBox_inCases->setCompleter(casesearchCompleter);

    ui->dateEdit_inv->setDate(today);

    //TRUST FUNDS
    trustmodel = new QSqlQueryModel();
    trustmodel->setQuery("SELECT DISTINCT clientname FROM trust_transactions WHERE archived = '0' ", db.db);
    setupTrustFundsTable();

    //transactions
    db.tts = new QSqlTableModel(this, db.db);
    db.tts->setTable("trust_transactions");
    db.tts->setFilter("archived = '0' ");
    db.tts->select();
    setupTrustTransactionsTable();

    //date
    ui->dateEdit_dateTF->setDate(QDate::currentDate());

    //CUSTODY ITEMS
    db.citems = new QSqlTableModel(this, db.db);
    db.citems->setTable("custody_items");
    db.citems->setFilter("archived = '0' ");
    db.citems->select();
    setupCustodyItemsTable();
    //clients comboBox
    ui->comboBox_ciClientFilter->setModel(clientsModel);
    ui->comboBox_ciClientFilter->setCompleter(csearchCompleter);

}

Dashboard::~Dashboard()
{
    delete ui;
}

/*
Start and pause timer
*/
void Dashboard::on_pushButton_startPause_clicked()
{
    if(start){
        //timer running, set button to pause
        if(timerStarted == false || timerPaused){
            timer->start(1000);
            timerStarted = true;
            timerStopped = false;
            timerPaused = false;
        }
        start = false;
        ui->pushButton_startPause->setText("pause");
        ui->pushButton_startPause->setIcon(QIcon(QString::fromUtf8( ":/new/general/Gnome-Media-Playback-Pause-64.png" )));
        pause = true;

    }else{
        //timer paused, set button to start
        timer->stop();
        timerPaused = true;
        start = true;
        ui->pushButton_startPause->setText("start");
        ui->pushButton_startPause->setIcon(QIcon(QString::fromUtf8( ":/new/general/Gnome-Media-Playback-Start-64.png" )));
        pause = false;
    }
}

/*
Setup tasks table
*/
void Dashboard::setupTasksTable(QTableView *tasksTable, QSqlTableModel *tasks)
{
    tasks->setHeaderData(3, Qt::Horizontal, tr("Task name"));
    tasks->setHeaderData(4, Qt::Horizontal, tr("Priority"));
    tasks->setHeaderData(5, Qt::Horizontal, tr("Status"));
    tasks->setHeaderData(6, Qt::Horizontal, tr("Description"));
    tasks->setHeaderData(7, Qt::Horizontal, tr("Date Created"));
    tasks->setHeaderData(8, Qt::Horizontal, tr("Created by"));
    tasks->setHeaderData(9, Qt::Horizontal, tr("Due date"));

    tasksTable->setModel(tasks);

    tasksTable->hideColumn(0); // don't show the taskid
    tasksTable->hideColumn(1); // don't show the caseid
    tasksTable->hideColumn(2); // don't show the clientid
    tasksTable->hideColumn(10); // don't show the last_modified
    tasksTable->hideColumn(11); // don't show the modified_by
    tasksTable->hideColumn(12); // don't show the rate
    tasksTable->hideColumn(13); // don't show the billtype
    tasksTable->hideColumn(14); // don't show the billhrs
    tasksTable->hideColumn(15); // don't show the non_billhrs
    tasksTable->hideColumn(16); // don't show archived

    tasksTable->setColumnWidth(6, 250);
    tasksTable->setColumnWidth(7, 150);
    tasksTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    tasksTable->horizontalHeader()->show();
    tasksTable->verticalHeader()->show();
    tasksTable->show();
}

/*
Setup clients table
*/
void Dashboard::setupClientsTable(QTableView *clientsTable, QSqlTableModel *clients)
{
    clients->setHeaderData(1, Qt::Horizontal, tr("Client name"));
    clients->setHeaderData(2, Qt::Horizontal, tr("Email"));
    clients->setHeaderData(3, Qt::Horizontal, tr("Phone No."));
    clients->setHeaderData(4, Qt::Horizontal, tr("Address"));
    clients->setHeaderData(10, Qt::Horizontal, tr("Status"));
    clients->setHeaderData(11, Qt::Horizontal, tr("Date created"));
    clients->setHeaderData(12, Qt::Horizontal, tr("Created by"));
    clients->setHeaderData(15, Qt::Horizontal, tr("DOB / Start date"));
    clients->setHeaderData(17, Qt::Horizontal, tr("Referral"));
    clients->setHeaderData(18, Qt::Horizontal, tr("Occupation"));
    clients->setHeaderData(21, Qt::Horizontal, tr("Business type"));
    clients->setHeaderData(22, Qt::Horizontal, tr("Contact name"));
    clients->setHeaderData(23, Qt::Horizontal, tr("Contact designation"));
    clients->setHeaderData(24, Qt::Horizontal, tr("Contact phone no"));
    clients->setHeaderData(25, Qt::Horizontal, tr("Email"));

    clientsTable->setModel(clients);

    clientsTable->hideColumn(0); // don't show the clientid
    clientsTable->hideColumn(5); //don't show address 2
    clientsTable->hideColumn(6); // don't show the city
    clientsTable->hideColumn(7); // don't show the state
    clientsTable->hideColumn(8); // don't show the po box
    clientsTable->hideColumn(9); // don't show the country
    clientsTable->hideColumn(13); // don't show the last_modified
    clientsTable->hideColumn(14); // don't show the modified_by
    clientsTable->hideColumn(16); // don't show id path
    clientsTable->hideColumn(19); // don't show bill type
    clientsTable->hideColumn(20); // don't show payment method
    clientsTable->hideColumn(26); // don't show archived

    clientsTable->setColumnWidth(2, 150);
    clientsTable->setColumnWidth(4, 250);
    clientsTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    clientsTable->horizontalHeader()->show();
    clientsTable->verticalHeader()->show();
    clientsTable->show();
}

/*
Setup cases table
*/
void Dashboard::setupCasesTable(QTableView *casesTable, QSqlTableModel *cases)
{
    cases->setHeaderData(1, Qt::Horizontal, tr("Case No."));
    cases->setHeaderData(2, Qt::Horizontal, tr("Case"));
    cases->setHeaderData(14, Qt::Horizontal, tr("Client"));
    cases->setHeaderData(3, Qt::Horizontal, tr("Date opened"));
    cases->setHeaderData(4, Qt::Horizontal, tr("Date closed"));
    cases->setHeaderData(5, Qt::Horizontal, tr("Status"));
    cases->setHeaderData(6, Qt::Horizontal, tr("Practice area"));
    cases->setHeaderData(12, Qt::Horizontal, tr("Created by"));
    cases->setHeaderData(13, Qt::Horizontal, tr("Closed by"));
    cases->setHeaderData(15, Qt::Horizontal, tr("Description"));

    casesTable->setModel(cases);

    casesTable->hideColumn(0); // don't show the caseid
    casesTable->hideColumn(7); //don't show the bill type
    casesTable->hideColumn(8); // don't show the rate
    casesTable->hideColumn(9); // don't show the billhrs
    casesTable->hideColumn(10); // don't show the non_billhrs
    casesTable->hideColumn(11); // don't show the po box
    casesTable->hideColumn(16); // don't show archived

    casesTable->setColumnWidth(2, 150);
    casesTable->setColumnWidth(6, 150);
    casesTable->setColumnWidth(14, 150);
    casesTable->setColumnWidth(15, 250);
    casesTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    casesTable->horizontalHeader()->show();
    casesTable->verticalHeader()->show();
    casesTable->show();
}

/*
Setup documents table
*/
void Dashboard::setupDocsTable(QTableView *docsTable, QFileSystemModel *files)
{
    files->setHeaderData(0, Qt::Horizontal, tr("Name"));
    files->setHeaderData(1, Qt::Horizontal, tr("Type"));
    files->setHeaderData(2, Qt::Horizontal, tr("Size"));
    files->setHeaderData(3, Qt::Horizontal, tr("Date modified"));

    docsTable->setModel(files);

    docsTable->setColumnWidth(0, 300);
    docsTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    docsTable->horizontalHeader()->show();
    docsTable->show();
}

/*
 * Setup time entries table
 */
void Dashboard::setupTimeEntriesTable()
{
    db.tentries->setHeaderData(4, Qt::Horizontal, tr("Description"));
    db.tentries->setHeaderData(5, Qt::Horizontal, tr("Date_created"));
    db.tentries->setHeaderData(6, Qt::Horizontal, tr("Created_by"));
    db.tentries->setHeaderData(7, Qt::Horizontal, tr("Last modified"));
    db.tentries->setHeaderData(8, Qt::Horizontal, tr("Modified_by"));
    db.tentries->setHeaderData(9, Qt::Horizontal, tr("Billable hours"));
    db.tentries->setHeaderData(10, Qt::Horizontal, tr("Non billable hours"));
    db.tentries->setHeaderData(11, Qt::Horizontal, tr("Rate"));
    db.tentries->setHeaderData(12, Qt::Horizontal, tr("Amount"));
    db.tentries->setHeaderData(13, Qt::Horizontal, tr("Bill type"));

    ui->tableView_tEntries->setModel(db.tentries);

    ui->tableView_tEntries->hideColumn(0); // don't show the time_entryid
    ui->tableView_tEntries->hideColumn(1); //don't show the userid
    ui->tableView_tEntries->hideColumn(2); // don't show the caseid
    ui->tableView_tEntries->hideColumn(3); // don't show the clientid
    ui->tableView_tEntries->hideColumn(14); // don't show archived

    ui->tableView_tEntries->setColumnWidth(4, 250);
    ui->tableView_tEntries->setColumnWidth(10, 150);
    ui->tableView_tEntries->setColumnWidth(11, 150);
    ui->tableView_tEntries->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableView_tEntries->horizontalHeader()->show();
    ui->tableView_tEntries->verticalHeader()->show();
    ui->tableView_tEntries->show();
}

/*
 * Setup expenses table
 */
void Dashboard::setupExpensesTable()
{
    db.exp->setHeaderData(1, Qt::Horizontal, tr("Expense type"));
    db.exp->setHeaderData(2, Qt::Horizontal, tr("Description"));
    db.exp->setHeaderData(3, Qt::Horizontal, tr("Amount"));
    db.exp->setHeaderData(4, Qt::Horizontal, tr("Created _by"));
    db.exp->setHeaderData(5, Qt::Horizontal, tr("Date_created"));
    db.exp->setHeaderData(6, Qt::Horizontal, tr("Confirmed by"));
    db.exp->setHeaderData(7, Qt::Horizontal, tr("Date_confirmed"));
    db.exp->setHeaderData(9, Qt::Horizontal, tr("Client"));
    db.exp->setHeaderData(10, Qt::Horizontal, tr("Case"));

    ui->tableView_expenses->setModel(db.exp);

    ui->tableView_expenses->hideColumn(0); // don't show the expense_id
    ui->tableView_expenses->hideColumn(8); //don't show archived
    ui->tableView_expenses->hideColumn(11); //don't show receiver

    ui->tableView_expenses->setColumnWidth(2, 250);
    ui->tableView_expenses->setColumnWidth(9, 150);
    ui->tableView_expenses->setColumnWidth(10, 150);
    ui->tableView_expenses->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableView_expenses->horizontalHeader()->show();
    ui->tableView_expenses->verticalHeader()->show();
    ui->tableView_expenses->show();
}

/*
 * Setup invoices table
 */
void Dashboard::setupInvoicesTable()
{
    db.invoices->setHeaderData(1, Qt::Horizontal, tr("Invoice No."));
    db.invoices->setHeaderData(2, Qt::Horizontal, tr("Client"));
    db.invoices->setHeaderData(3, Qt::Horizontal, tr("Case"));
    db.invoices->setHeaderData(4, Qt::Horizontal, tr("VAT"));
    db.invoices->setHeaderData(5, Qt::Horizontal, tr("Notes"));
    db.invoices->setHeaderData(6, Qt::Horizontal, tr("Status"));
    db.invoices->setHeaderData(7, Qt::Horizontal, tr("Created by"));
    db.invoices->setHeaderData(8, Qt::Horizontal, tr("Date created"));
    db.invoices->setHeaderData(9, Qt::Horizontal, tr("Rate"));
    db.invoices->setHeaderData(10, Qt::Horizontal, tr("Billable hours"));
    db.invoices->setHeaderData(11, Qt::Horizontal, tr("Non billable hours"));
    db.invoices->setHeaderData(12, Qt::Horizontal, tr("Bill type"));
    db.invoices->setHeaderData(13, Qt::Horizontal, tr("Service amount"));
    db.invoices->setHeaderData(14, Qt::Horizontal, tr("Expense amount"));
    db.invoices->setHeaderData(15, Qt::Horizontal, tr("Payment method"));
    db.invoices->setHeaderData(16, Qt::Horizontal, tr("Date paid"));
    db.invoices->setHeaderData(17, Qt::Horizontal, tr("Amount"));

    ui->tableView_invoices->setModel(db.invoices);

    ui->tableView_invoices->hideColumn(0); // don't show the uid
    ui->tableView_invoices->hideColumn(18); //don't show archived

    ui->tableView_invoices->setColumnWidth(2, 150);
    ui->tableView_invoices->setColumnWidth(3, 150);
    ui->tableView_invoices->setColumnWidth(5, 250);
    ui->tableView_invoices->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableView_invoices->horizontalHeader()->show();
    ui->tableView_invoices->verticalHeader()->show();
    ui->tableView_invoices->show();
}

/*
 * Setup trust funds table
 */
void Dashboard::setupTrustFundsTable()
{
    trustmodel->setHeaderData(0, Qt::Horizontal, tr("Client"));

    ui->tableView_trustFunds->setModel(trustmodel);

    ui->tableView_trustFunds->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableView_trustFunds->horizontalHeader()->show();
    ui->tableView_trustFunds->verticalHeader()->show();
    ui->tableView_trustFunds->show();
}

/*
 * Setup trust transactions table
 */
void Dashboard::setupTrustTransactionsTable()
{
    db.tts->setHeaderData(1, Qt::Horizontal, tr("Transaction"));
    db.tts->setHeaderData(2, Qt::Horizontal, tr("Client"));
    db.tts->setHeaderData(3, Qt::Horizontal, tr("Date created"));
    db.tts->setHeaderData(4, Qt::Horizontal, tr("Amount"));
    db.tts->setHeaderData(5, Qt::Horizontal, tr("Created by"));
    db.tts->setHeaderData(6, Qt::Horizontal, tr("Payment method"));

    ui->tableView_transactions->setModel(db.tts);

    ui->tableView_transactions->setColumnWidth(1, 150);
    ui->tableView_transactions->setColumnWidth(2, 150);
    ui->tableView_transactions->setColumnWidth(4, 150);
    ui->tableView_transactions->setColumnWidth(5, 150);
    ui->tableView_transactions->setColumnWidth(6, 150);

    ui->tableView_transactions->hideColumn(0); //don't show uid
    ui->tableView_transactions->hideColumn(7); //don't show archived

    ui->tableView_transactions->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableView_transactions->horizontalHeader()->show();
    ui->tableView_transactions->verticalHeader()->show();
    ui->tableView_transactions->show();
}

/*
 * Setup custody items
 */
void Dashboard::setupCustodyItemsTable()
{
    db.citems->setHeaderData(1, Qt::Horizontal, tr("Item ID"));
    db.citems->setHeaderData(2, Qt::Horizontal, tr("Item name"));
    db.citems->setHeaderData(3, Qt::Horizontal, tr("Quantity"));
    db.citems->setHeaderData(4, Qt::Horizontal, tr("Description"));
    db.citems->setHeaderData(5, Qt::Horizontal, tr("Location"));
    db.citems->setHeaderData(6, Qt::Horizontal, tr("Date logged"));
    db.citems->setHeaderData(7, Qt::Horizontal, tr("Logged by"));
    db.citems->setHeaderData(8, Qt::Horizontal, tr("Review date"));
    db.citems->setHeaderData(9, Qt::Horizontal, tr("Reminder date"));
    db.citems->setHeaderData(12, Qt::Horizontal, tr("Client"));

    ui->tableView_ci->setModel(db.citems);

    ui->tableView_ci->setColumnWidth(2, 150);
    ui->tableView_ci->setColumnWidth(4, 250);
    ui->tableView_ci->setColumnWidth(7, 150);

    ui->tableView_ci->hideColumn(0); //hide uid
    ui->tableView_ci->hideColumn(10); //hide image path
    ui->tableView_ci->hideColumn(11); //hide archived

    ui->tableView_transactions->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableView_transactions->horizontalHeader()->show();
    ui->tableView_transactions->verticalHeader()->show();
    ui->tableView_transactions->show();
}

/*
Stop timer
*/
void Dashboard::on_pushButton_stop_clicked()
{
    timer->stop();
    secCounter = 0;
    timerStarted = false;
    timerStopped = true;
    start = true;
    ui->pushButton_startPause->setText("start");
    ui->pushButton_startPause->setIcon(QIcon(QString::fromUtf8( ":/new/general/Gnome-Media-Playback-Start-64.png" )));
    pause = false;
    ui->label_time->setText("00:00:00");
}

/*
Update the timer every second
*/
void Dashboard::updateTime()
{
    //update seconds counter
    secCounter++;

    //save hrs, mins, seconds
    int secs = secCounter;
    int mins = secs/60;
    int hrs = 0;

    //reset seconds if greater than 59
    if(secs>59)
        secs = secs%60;

    //update the  timer
    if(mins > 60){
        hrs = mins/60;
        mins = mins%60;

        QString hrstr = QString::number(hrs);
        QString minstr = QString::number(mins);
        QString secstr = QString::number(secs);

        if(secs<10){
            if(hrs<10)
                ui->label_time->setText("0"%hrstr%":"%minstr%":0"%secstr);
            else
                ui->label_time->setText(hrstr%":"%minstr%":0"%secstr);
        }else{
            if(hrs<10)
                ui->label_time->setText("0"%hrstr%":"%minstr%":"%secstr);
            else
                ui->label_time->setText(hrstr%":"%minstr%":"%secstr);
        }
    }else{
        if(mins<10){
            QString minstr = QString::number(mins);
            QString secstr = QString::number(secs);

            if(secs<10){
                ui->label_time->setText("00:0"%minstr%":0"%secstr);
            }else
                ui->label_time->setText("00:0"%minstr%":"%secstr);
        }else{
            QString minstr = QString::number(mins);
            QString secstr = QString::number(secs);

            if(secs<10)
                ui->label_time->setText("00:"%minstr%":0"%secstr);
            else
                ui->label_time->setText("00:"%minstr%":"%secstr);
        }
    }
}

/*
Refresh upcoming tasks
*/
void Dashboard::on_pushButton_refreshHome_clicked()
{
    db.utasks->setTable("tasks");
    db.utasks->setFilter("status = 'pending' AND archived = '0' ");
    db.utasks->select();
    db.utasks->setEditStrategy(QSqlTableModel::OnManualSubmit);
    setupTasksTable(ui->tableView_upcomingTasks, db.utasks);
}

/*
Search upcoming tasks
*/
void Dashboard::on_pushButton_searchUTasks_clicked()
{
    QString searchstr = ui->lineEdit_searchUTasks->text();
    search_utasks(searchstr);
}

/*
Search upcoming tasks function
*/
void Dashboard::search_utasks(QString &taskname)
{
    db.utasks->setTable("tasks");
    db.utasks->setFilter("taskname LIKE '%"%taskname%"%' AND status = 'pending' ");
    db.utasks->select();
    setupTasksTable(ui->tableView_upcomingTasks, db.utasks);
}

/*
Search upcoming tasks on return pressed
*/
void Dashboard::on_lineEdit_searchUTasks_returnPressed()
{
    QString completion = utsearchcompleter->currentCompletion();
    QString searchstr = ui->lineEdit_searchUTasks->text();

    if(completion != NULL)
        search_utasks(completion);
    else
        search_utasks(searchstr);
}

/*
New time entry
*/
void Dashboard::on_pushButton_newTime_clicked()
{
    int hrs, mins;
    QString timestr = ui->label_time->text();
    hf.getTime(timestr);

    NewTimeEntry *nt = new NewTimeEntry();
    hrs = hf.getHrs();
    mins = hf.getMins();

    nt->setTime(hrs, mins);
    nt->show();
}

/*
Navigation menu
*/
void Dashboard::on_listWidget_menu_clicked(const QModelIndex &index)
{
    //show matching stackwidget page
    int selection = index.row();

    switch(selection){
    case 0:
        ui->stackedWidget->setCurrentIndex(0);
        break;

    case 1:
        //home
        ui->stackedWidget->setCurrentIndex(1);
        break;

    case 2:
        //calender
        ui->stackedWidget->setCurrentIndex(2);
        break;

    case 3:
        //tasks
        ui->stackedWidget->setCurrentIndex(3);
        break;

    case 4:
        //clients
        ui->stackedWidget->setCurrentIndex(4);
        break;

    case 5:
        //cases
        ui->stackedWidget->setCurrentIndex(5);
        break;

    case 6:
        //documents
        ui->stackedWidget->setCurrentIndex(6);
        break;

    case 7:
        //billing
        ui->stackedWidget->setCurrentIndex(7);
        break;

    case 8:
        //trust funds
        ui->stackedWidget->setCurrentIndex(8);
        break;

    case 9:
        //custody items
        ui->stackedWidget->setCurrentIndex(9);
        break;

    default:
        ui->stackedWidget->setCurrentIndex(0);
        break;
    }
}

/*
Show my tasks
*/
void Dashboard::on_pushButton_smTasks_clicked()
{
    QString user = db.getCurrentUserFullName();

    //get userid from fullname
    int userid = db.getUserID(user);

    //get taskids of tasks user is assigned to
    QSqlQuery query(db.db);
    query.prepare(QString("SELECT taskid "
                          "FROM user_tasks WHERE "
                          "userid = :userid"));
    query.bindValue(":userid", userid);
    query.exec();

    QStringList taskids;
    QString taskid;

    //add taskids to string array
    while (query.next()) {
        taskid = query.value(0).toString();
        taskids.append(taskid);
    }

    //separate taskids by commas and make it one string
    QString tstr = taskids.join(", ");

    //use that string as a filter for the tasks the user is assigned to
    db.tasks->setTable("tasks");
    db.tasks->setFilter("taskid in ("%tstr%") ");
    db.tasks->select();
    setupTasksTable(ui->tableView_tasks, db.tasks);
}

/*
Search tasks on return pressed
*/
void Dashboard::on_lineEdit_searchTasks_returnPressed()
{
    QString completion = tsearchcompleter->currentCompletion();
    QString searchstr = ui->lineEdit_searchTasks->text();

    if(completion != NULL)
        search_tasks(completion);
    else
        search_tasks(searchstr);
}

/*
Search tasks function
*/
void Dashboard::search_tasks(QString &taskname)
{
    db.tasks->setTable("tasks");
    db.tasks->setFilter("taskname LIKE '%"%taskname%"%'");
    db.tasks->select();
    setupTasksTable(ui->tableView_tasks, db.tasks);
}

/*
Search tasks button clicked
*/
void Dashboard::on_pushButton_searchTasks_clicked()
{
    QString searchquery = ui->lineEdit_searchTasks->text();
    search_tasks(searchquery);
}

/*
Refresh tasks
*/
void Dashboard::on_pushButton_refreshTasks_clicked()
{
    db.tasks->setTable("tasks");
    db.tasks->setFilter("archived = '0' ");
    db.tasks->select();
    setupTasksTable(ui->tableView_tasks, db.tasks);
}

/*
Show tasks assigned to...
*/
void Dashboard::on_comboBox_stat_currentIndexChanged(const QString &arg1)
{
    //get userid from fullname
    int userid = db.getUserID(arg1);

    //get taskids of tasks user is assigned to
    QSqlQuery query(db.db);
    query.prepare(QString("SELECT taskid "
                          "FROM user_tasks WHERE "
                          "userid = :userid"));
    query.bindValue(":userid", userid);
    query.exec();

    QStringList taskids;
    QString taskid;

    //add taskids to string array
    while (query.next()) {
        taskid = query.value(0).toString();
        taskids.append(taskid);
    }

    //separate taskids by commas and make it one string
    QString tstr = taskids.join(", ");

    //use that string as a filter for the tasks the user is assigned to
    db.tasks->setTable("tasks");
    db.tasks->setFilter("taskid in ("%tstr%") ");
    db.tasks->select();
    setupTasksTable(ui->tableView_tasks, db.tasks);
}

/*
Get users asigned to selected task
*/
void Dashboard::on_tableView_tasks_clicked(const QModelIndex &index)
{
    selectedTaskRow = index.row();
    QString taskid = db.tasks->index(selectedTaskRow, 0).data().toString();

    QSqlQuery query(db.db);
    query.prepare(QString("SELECT userid "
                          "FROM user_tasks WHERE "
                          "taskid = :taskid"));
    query.bindValue(":taskid", taskid);
    query.exec();

    QStringList userids;
    QString userid;

    while (query.next()) {
        userid = query.value(0).toString();
        userids.append(userid);
    }

    QString ustr = userids.join(", ");

    QSqlQueryModel *uatmodel = new QSqlQueryModel();
    uatmodel->setQuery("SELECT fullname FROM users WHERE userid in ("%ustr%") ", db.db);
    ui->listView_uat->setModel(uatmodel);
}

/*
Get pending tasks
*/
void Dashboard::on_pushButton_pending_clicked()
{
    db.tasks->setTable("tasks");
    db.tasks->setFilter("status = 'pending' AND archived = '0' ");
    db.tasks->select();
    setupTasksTable(ui->tableView_tasks, db.tasks);
}

/*
Get finished tasks
*/
void Dashboard::on_pushButton_finished_clicked()
{
    db.tasks->setTable("tasks");
    db.tasks->setFilter("status = 'finished' AND archived = '0' ");
    db.tasks->select();
    setupTasksTable(ui->tableView_tasks, db.tasks);
}

/*
Filter tasks by date
*/
void Dashboard::on_pushButton_filterTasksByDate_clicked()
{
    QString from = ui->dateEdit_tFrom->date().toString("yyyy/MM/dd");
    QString to = ui->dateEdit_tTo->date().toString("yyyy/MM/dd");

    db.tasks->setTable("tasks");
    db.tasks->setFilter("(date_created BETWEEN ('"%from%"', '"%to%"') AND archived = '0' ");
    db.tasks->select();
    setupTasksTable(ui->tableView_tasks, db.tasks);
}

/*
Filter tasks by case
*/
void Dashboard::on_comboBox_tCases_currentIndexChanged(const QString &arg1)
{
    QSqlQuery query(db.db);
    query.prepare(QString("SELECT caseid "
                          "FROM cases WHERE "
                          "casename = :casename"));
    query.bindValue(":casename", arg1);
    query.exec();

    QString caseid;

    while (query.next()) {
        caseid = query.value(0).toString();
    }

    db.tasks->setTable("tasks");
    db.tasks->setFilter("caseid = ' "%caseid%" ' ");
    db.tasks->select();
    setupTasksTable(ui->tableView_tasks, db.tasks);
}

/*
Filter tasks by priority
*/
void Dashboard::on_comboBox_priority_currentIndexChanged(const QString &arg1)
{
    db.tasks->setTable("tasks");
    db.tasks->setFilter("priority = '"%arg1%"'");
    db.tasks->select();
    setupTasksTable(ui->tableView_tasks, db.tasks);
}

//New task
void Dashboard::on_pushButton_newTask_clicked()
{
    newtask *nt = new newtask;
    nt->show();
}

/*
Right click on tasks
*/
void Dashboard::on_tableView_tasks_customContextMenuRequested(const QPoint &pos)
{
    QMenu *menu = new QMenu(this);
    menu->addAction(new QAction("Add user to task", this));
    menu->addAction(new QAction("Mark as finished", this));
    menu->addAction(new QAction("Mark as pending", this));
    menu->addAction(new QAction("Delete task", this));
    connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(menuSelection(QAction*)));
    menu->exec(ui->tableView_tasks->mapToGlobal(pos));
}

/*
Menu selection slot
*/
void Dashboard::menuSelection(QAction* action)
{
    QModelIndexList list = ui->tableView_tasks->selectionModel()->selectedRows(0);

    QString addUser = "Add user to task";
    QString finished = "Mark as finished";
    QString pending = "Mark as pending";
    QString deleteTask = "Delete task";

    int selection = 0;

    if(action->text() == addUser)
        selection = 1;
    if(action->text() == finished)
        selection = 2;
    if(action->text() == pending)
        selection = 3;
    if(action->text() == deleteTask)
        selection = 4;

    //add user to task
    if(selection == 1){
        //get task names
        QModelIndexList tnames = ui->tableView_tasks->selectionModel()->selectedRows(3);

        UserTask *ut = new UserTask;
        QString taskname;
        QStringList tasknames;

        for(int i=0; i<tnames.count(); i++){
            taskname = tnames.at(i).data().toString();
            tasknames.append(taskname);
        }

        //show add user to task dialog
        if(tasknames.isEmpty()){
            QMessageBox::information(0, qApp->tr("An error occured!"),
                                     qApp->tr("No tasks selected.\n"),
                                     QMessageBox::Close);
        }else{
            ut->setTasknames(tasknames);
            ut->show();
        }
    }

    //mark selection as finished
    if(selection == 2){
        QSqlQuery query(db.db);

        for(int i=0; i<list.count(); i++){
            query.prepare(QString("UPDATE tasks SET status = 'finished' WHERE taskid = :taskid"));
            query.bindValue(":taskid", list.at(i).data().toInt());
            query.exec();
            qDebug()<<query.lastError().text();
        }

        ui->pushButton_refreshTasks->click();
    }

    //mark selection as pending
    if(selection == 3){
        QSqlQuery query(db.db);

        for(int i=0; i<list.count(); i++){
            query.prepare(QString("UPDATE tasks SET status = 'pending' WHERE taskid = :taskid"));
            query.bindValue(":taskid", list.at(i).data().toInt());
            query.exec();
            qDebug()<<query.lastError().text();
        }
        ui->pushButton_refreshTasks->click();
    }

    //delete task
    if(selection == 4){
        QSqlQuery query(db.db);

        for(int i=0; i<list.count(); i++){
            query.prepare(QString("UPDATE tasks SET archived = '1' WHERE taskid = :taskid"));
            query.bindValue(":taskid", list.at(i).data().toInt());
            query.exec();
        }
        ui->pushButton_refreshTasks->click();
    }
}

/*
Delete selected tasks
*/
void Dashboard::on_pushButton_deleteTask_clicked()
{
    //get selected task names
    QModelIndexList tnames = ui->tableView_tasks->selectionModel()->selectedRows(3);

    QMessageBox::StandardButton reply;

    if(isItemSelected(ui->tableView_tasks))
    {
        reply = QMessageBox::question(this, "Delete tasks",
                                      "Are you sure you want to delete these tasks?\n",
                                      QMessageBox::Yes | QMessageBox::No);

        if(reply == QMessageBox::Yes){
            for(int i=0; i<tnames.count(); i++){
                db.deleteTask(tnames.at(i).data().toString());
            }
        }
    }

    ui->pushButton_refreshTasks->click();
}

/*
 * Mark upcoming task as finished
*/
void Dashboard::on_pushButton_markFinished_clicked()
{
    QModelIndexList list = ui->tableView_upcomingTasks->selectionModel()->selectedRows(0);
    QSqlQuery query(db.db);

    for(int i=0; i<list.count(); i++){
        query.prepare(QString("UPDATE tasks SET status = 'finished' WHERE taskid = :taskid"));
        query.bindValue(":taskid", list.at(i).data().toInt());

        if(!query.exec()) {
            qDebug()<<query.lastError().text();
        }
    }

    ui->pushButton_refreshHome->click();
}

/*
 * Delete selected upcoming tasks
*/
void Dashboard::on_pushButton_delUtask_clicked()
{
    //get selected task names
    QModelIndexList tnames = ui->tableView_upcomingTasks->selectionModel()->selectedRows(3);

    if(isItemSelected(ui->tableView_upcomingTasks)){
        QMessageBox::StandardButton reply;

        reply = QMessageBox::question(this, "Delete tasks",
                                      "Are you sure you want to delete these tasks?\n",
                                      QMessageBox::Yes | QMessageBox::No);

        if(reply == QMessageBox::Yes){
            for(int i=0; i<tnames.count(); i++){
                db.deleteTask(tnames.at(i).data().toString());
            }
        }
    }

    ui->pushButton_refreshHome->click();
}

/*
 * Filter upcoming tasks by date
 */
void Dashboard::on_pushButton_filterUTByDate_clicked()
{
    QString from = ui->dateEdit_fromUtasks->date().toString("yyyy/MM/dd");
    QString to = ui->dateEdit_toUtasks->date().toString("yyyy/MM/dd");

    db.utasks->setTable("tasks");
    db.utasks->setFilter("(date_created BETWEEN ('"%from%"',' '"%to%"') AND status = 'pending' "
                                                                    "AND archived = '0' ");
    db.utasks->select();
    setupTasksTable(ui->tableView_upcomingTasks, db.utasks);
}

/*
 * Filter upcoming tasks by case
*/
void Dashboard::on_comboBox_filterUtaskCase_activated(const QString &arg1)
{
    QSqlQuery query(db.db);
    query.prepare(QString("SELECT caseid "
                          "FROM cases WHERE "
                          "casename LIKE %"+arg1+"%"));
    query.exec();

    QString caseid;

    while (query.next()) {
        caseid = query.value(0).toString();
    }

    db.utasks->setTable("tasks");
    db.utasks->setFilter("caseid = ' "%caseid%" ' ");
    db.utasks->select();
    setupTasksTable(ui->tableView_upcomingTasks, db.utasks);
}

/*
 * Filter upcoming tasks by priority
*/
void Dashboard::on_comboBox_filterUtaskPriority_activated(const QString &arg1)
{
    db.utasks->setTable("tasks");
    db.utasks->setFilter("priority = '"%arg1%"'");
    db.utasks->select();
    setupTasksTable(ui->tableView_upcomingTasks, db.utasks);
}

/*
 * New task (upcoming tasks)
*/
void Dashboard::on_pushButton_newTask_2_clicked()
{
    newtask *ntask = new newtask();
    ntask->show();
}

/*
 * Show tasks on clicked date
 */
void Dashboard::on_calendarWidget_clicked(const QDate &date)
{
    db.ctasks->setTable("tasks");
    db.ctasks->setFilter("(date_created = '"%date.toString("yyyy-MM-dd")%"' OR due_date = '"%date.toString("yyyy-MM-dd")%"') "
                                                                                                                         "AND archived = '0' ");
    db.ctasks->select();
    ui->listView_tasks->setModel(db.ctasks);
    ui->listView_tasks->setModelColumn(3);
}

/*
 * Show tasks in selected month
 */
void Dashboard::on_comboBox_monthFilter_activated(int index)
{
    int year = ui->calendarWidget->selectedDate().year();

    QString date = QString::number(year)%"-"%QString::number(index+1)%"-1";
    QString date2;
    if((index+2) > 12){
        date2 = QString::number(year)%"-"%QString::number(index+1)%"-30";
    } else {
        date2 = QString::number(year)%"-"%QString::number(index+2)%"-1";
    }

    db.ctasks->setTable("tasks");
    db.ctasks->setFilter("(date_created BETWEEN #"%date%"# AND #"%date2%"#) AND archived = '0' ");
    db.ctasks->select();
    ui->listView_tasks->setModel(db.ctasks);
    ui->listView_tasks->setModelColumn(3);
}

/*
 * Show date of clicked task list item
 */
void Dashboard::on_listView_tasks_clicked(const QModelIndex &index)
{
    QDate selectedDate = db.ctasks->index(index.row(), 7).data().toDate();

    ui->calendarWidget->setSelectedDate(selectedDate);
}

/*
 * Refresh calendar tasks
 */
void Dashboard::on_pushButton_refreshCtasks_clicked()
{
    db.ctasks->setTable("tasks");
    db.ctasks->setFilter("archived = '0' ");
    db.ctasks->select();
    ui->listView_tasks->setModel(db.ctasks);
    ui->listView_tasks->setModelColumn(3);
}

/*
 * New task (calendar)
 */
void Dashboard::on_pushButton_newCtask_clicked()
{
    newtask *ntask = new newtask();
    ntask->show();
}

/*
 * Delete task (calendar)
 */
void Dashboard::on_pushButton_delCtask_clicked()
{
    //get selected task names
    QModelIndexList tnames = ui->listView_tasks->selectionModel()->selectedRows(0);

    if(!tnames.isEmpty()){
        QMessageBox::StandardButton reply;

        reply = QMessageBox::question(this, "Delete tasks",
                                      "Are you sure you want to delete these tasks?\n",
                                      QMessageBox::Yes | QMessageBox::No);

        if(reply == QMessageBox::Yes){
            for(int i=0; i<tnames.count(); i++){
                db.deleteTask(tnames.at(i).data().toString());
                qDebug() << tnames.at(i).data().toString();
            }
        }
    } else {
        QMessageBox::information(this, "No selection!",
                                 "Please select a task from the list on the right!", QMessageBox::Close);
    }

    ui->pushButton_refreshCtasks->click();
}

/*
 * Search clients
*/
void Dashboard::on_pushButton_searchClients_clicked()
{
    QString cname = ui->lineEdit_searchClients->text();
    searchClients(cname);
}

/*
 * New client
*/
void Dashboard::on_pushButton_newClient_clicked()
{
    newclient *newClient = new newclient();
    newClient->show();
}

/*
 * Refresh clients
*/
void Dashboard::on_pushButton_refreshClients_clicked()
{
    db.clients->setTable("clients");
    db.clients->setFilter("archived = '0' ");
    db.clients->select();
    setupClientsTable(ui->tableView_clients, db.clients);
}

/*
 * Search clients
*/
void Dashboard::searchClients(QString &clientname)
{
    db.clients->setTable("clients");
    db.clients->setFilter("clientname LIKE '%"%clientname%"%' AND archived = '0' ");
    db.clients->select();
    setupClientsTable(ui->tableView_clients, db.clients);
}

/*
 * Show selected client image
*/
void Dashboard::on_tableView_clients_clicked(const QModelIndex &index)
{
    QString path = db.clients->index(index.row(), 16).data().toString();

    ui->frame_clientID->setStyleSheet("image: url("%path%");"
                                                         "background-color: white;");
}

/*
 * Show my clients
*/
void Dashboard::on_pushButton_myClients_clicked()
{
    QString user = db.getCurrentUserFullName();

    db.clients->setTable("clients");
    db.clients->setFilter("created_by = '"%user%"' AND archived = '0' ");
    db.clients->select();
    setupClientsTable(ui->tableView_clients, db.clients);
}

/*
 * Show clients assigned to chosen user
*/
void Dashboard::on_comboBox_users_activated(const QString &arg1)
{
    db.clients->setTable("clients");
    db.clients->setFilter("created_by = '"%arg1%"' AND archived = '0' ");
    db.clients->select();
    setupClientsTable(ui->tableView_clients, db.clients);
}

/*
 * Delete clients
 */
void Dashboard::on_pushButton_delClient_clicked()
{
    QModelIndexList clientids = ui->tableView_clients->selectionModel()->selectedRows(0);

    QSqlQuery query(db.db);

    QMessageBox::StandardButton reply;

    if(isItemSelected(ui->tableView_clients))
    {
        reply = QMessageBox::question(this, "Delete clients",
                                      "Are you sure you want to delete these clients?\n",
                                      QMessageBox::Yes | QMessageBox::No);

        if(reply == QMessageBox::Yes){
            for(int i=0; i<clientids.count(); i++){
                query.prepare(QString("UPDATE clients SET archived = '1' WHERE clientid = :clientid AND archived = '0' "));
                query.bindValue(":clientid", clientids.at(i).data().toInt());
                query.exec();
            }
        }
    }

    ui->pushButton_refreshClients->click();
}

/*
 * Show open cases
 */
void Dashboard::on_pushButton_openCases_clicked()
{
    db.cases->setTable("cases");
    db.cases->setFilter("status = 'Open' AND archived = '0' ");
    db.cases->select();
    setupCasesTable(ui->tableView_cases, db.cases);

}

/*
 * Show closed cases
 */
void Dashboard::on_pushButton_closedCases_clicked()
{
    db.cases->setTable("cases");
    db.cases->setFilter("status = 'Closed' AND archived = '0' ");
    db.cases->select();
    setupCasesTable(ui->tableView_cases, db.cases);
}

/*
 * Refresh cases
 */
void Dashboard::on_pushButton_refreshCases_clicked()
{
    db.cases->setTable("cases");
    db.cases->setFilter("archived = '0' ");
    db.cases->select();
    setupCasesTable(ui->tableView_cases, db.cases);
}

/*
 * Filter cases by practice area
 */
void Dashboard::on_comboBox_pAreas_activated(const QString &arg1)
{
    db.cases->setTable("cases");
    db.cases->setFilter("practice_area = '"%arg1%"' AND archived = '0' ");
    db.cases->select();
    setupCasesTable(ui->tableView_cases, db.cases);
}

/*
 * Show my cases
 */
void Dashboard::on_pushButton_myCases_clicked()
{
    QString user = db.getCurrentUserFullName();

    db.cases->setTable("cases");
    db.cases->setFilter("created_by = '"%user%"' AND archived = '0' ");
    db.cases->select();
    setupCasesTable(ui->tableView_cases, db.cases);
}

/*
 * Search cases on return pressed
 */
void Dashboard::on_lineEdit_searchCases_returnPressed()
{
    QString casename = ui->lineEdit_searchCases->text();
    searchCases(casename);
}
/*
 * Search cases on search button pressed
 */
void Dashboard::on_pushButton_searchCases_clicked()
{
    QString casename = ui->lineEdit_searchCases->text();
    searchCases(casename);
}

/*
 * Search cases function
 */
void Dashboard::searchCases(QString &casename)
{
    db.cases->setTable("cases");
    db.cases->setFilter("casename = '"%casename%"' AND archived = '0' ");
    db.cases->select();
    setupCasesTable(ui->tableView_cases, db.cases);
}

/*
 * Delete case
 */
void Dashboard::on_pushButton_delCase_clicked()
{
    QModelIndexList caseids = ui->tableView_cases->selectionModel()->selectedRows(0);

    QSqlQuery query(db.db);

    if(isItemSelected(ui->tableView_cases))
    {
        QMessageBox::StandardButton reply;

        reply = QMessageBox::question(this, "Delete cases",
                                      "Are you sure you want to delete these cases?\n",
                                      QMessageBox::Yes | QMessageBox::No);

        if(reply == QMessageBox::Yes){
            for(int i=0; i<caseids.count(); i++){
                query.prepare(QString("UPDATE cases SET archived = '1' WHERE caseid = :caseid"));
                query.bindValue(":caseid", caseids.at(i).data().toInt());
                query.exec();
            }
        }
    }

    ui->pushButton_refreshCases->click();
}

/*
 * Check if tableView has a selected item
 */
bool Dashboard::isItemSelected(QTableView *tableView)
{
    QModelIndexList selectedRows = tableView->selectionModel()->selectedRows();

    if(selectedRows.count() < 1){

        QMessageBox::information(this, "No item selected!",
                                 "Please select an item first!", QMessageBox::Close);

    } else {
        return true;
    }

    return false;
}

/*
 * New case
 */
void Dashboard::on_pushButton_newCase_clicked()
{
    newcase *ncase = new newcase();
    ncase->show();
}
/*
 * Selected path in treeView
 */
void Dashboard::on_treeView_clicked(const QModelIndex &index)
{
    QString sPath = fmodel->fileInfo(index).absoluteFilePath();
    ui->tableView_docs->setRootIndex(fmodel->setRootPath(sPath));
}

/*
 * Selected path in tableView
 */
void Dashboard::on_tableView_docs_doubleClicked(const QModelIndex &index)
{
    QString sPath = fmodel->fileInfo(index).absoluteFilePath();

    if(fmodel->isDir(index)) {
        ui->tableView_docs->setRootIndex(fmodel->setRootPath(sPath));
    }
}

/*
 * New time entry
 */
void Dashboard::on_pushButton_newTimeEntry_clicked()
{
    NewTimeEntry *nTimeEntry = new NewTimeEntry();
    nTimeEntry->show();
}

/*
 * Filter time entries by users
 */
void Dashboard::on_comboBox_teUsers_activated(const QString &arg1)
{
    db.tentries->setTable("time_entries");
    db.tentries->setFilter("created_by = '"%arg1%"' AND archived = '0' ");
    db.tentries->select();
    setupTimeEntriesTable();
}

/*
 * Filter time entries by client name
 */
void Dashboard::on_comboBox_teClients_activated(const QString &arg1)
{
    db.tentries->setTable("time_entries");
    db.tentries->setFilter("clientname = '"%arg1%"' AND archived = '0' ");
    db.tentries->select();
    setupTimeEntriesTable();
}

/*
 * Filter time entries by case name
 */
void Dashboard::on_comboBox_teCases_activated(const QString &arg1)
{
    db.tentries->setTable("time_entries");
    db.tentries->setFilter("casename = '"%arg1%"' AND archived = '0' ");
    db.tentries->select();
    setupTimeEntriesTable();
}

/*
 * Filter time entries by selected date
 */
void Dashboard::on_pushButton_teDateFilter_clicked()
{
    db.tentries->setTable("time_entries");
    db.tentries->setFilter("date_created = '"%ui->dateEdit_tEntries->date().toString("yyyy/MM/dd")%"' AND archived = '0' ");
    db.tentries->select();
    setupTimeEntriesTable();
}

/*
 * Refresh selected billing tab
 */
void Dashboard::on_pushButton_refreshBilling_clicked()
{
    int tab = ui->tabWidget_billing->currentIndex();

    switch(tab)
    {
    case 0:
        refreshDash();
        break;

    case 1:
        refreshTimeEntries();
        break;

    case 2:
        refreshExpenses();
        break;

    case 3:
        refreshInvoices();
        break;
    }
}

/*
 * Refresh dashboard
 */
void Dashboard::refreshDash()
{
    //
}

/*
 * Refresh time entries
 */
void Dashboard::refreshTimeEntries()
{
    db.tentries->setTable("time_entries");
    db.tentries->setFilter("archived = '0' ");
    db.tentries->select();
    setupTimeEntriesTable();
}

/*
 * Refresh expenses
 */
void Dashboard::refreshExpenses()
{
    db.exp = new QSqlTableModel(this, db.db);
    db.exp->setTable("expenses");
    db.exp->setFilter("archived = '0' ");
    db.exp->select();
    db.exp->setEditStrategy(QSqlTableModel::OnManualSubmit);
    setupExpensesTable();
}

/*
 * Refresh invoices
 */
void Dashboard::refreshInvoices()
{
    db.invoices = new QSqlTableModel(this, db.db);
    db.invoices->setTable("invoices");
    db.invoices->setFilter("archived = '0' ");
    db.invoices->select();
    setupInvoicesTable();
}

/*
 * New expense
 */
void Dashboard::on_pushButton_newExpense_clicked()
{
    NewExpense *nexpense = new NewExpense();
    nexpense->show();
}

/*
 * Filter expenses by user
 */
void Dashboard::on_comboBox_expUsers_activated(const QString &arg1)
{
    db.exp->setTable("expenses");
    db.exp->setFilter("created_by = '"%arg1%"' AND archived = '0' ");
    db.exp->select();
    setupExpensesTable();
}

/*
 * Filter expenses by expense type
 */
void Dashboard::on_comboBox_expType_activated(const QString &arg1)
{
    db.exp->setTable("expenses");
    db.exp->setFilter("expense_type = '"%arg1%"' AND archived = '0' ");
    db.exp->select();
    setupExpensesTable();
}

/*
 * Filter expenses by client name
 */
void Dashboard::on_comboBox_expClients_activated(const QString &arg1)
{
    db.exp->setTable("expenses");
    db.exp->setFilter("clientname = '"%arg1%"' AND archived = '0' ");
    db.exp->select();
    setupExpensesTable();
}

/*
 * Filter expenses by case name
 */
void Dashboard::on_comboBox_expCases_activated(const QString &arg1)
{
    db.exp->setTable("expenses");
    db.exp->setFilter("casename = '"%arg1%"' AND archived = '0' ");
    db.exp->select();
    setupExpensesTable();
}

/*
 * Filter expenses by date
 */
void Dashboard::on_pushButton_expDateFilter_clicked()
{
    db.exp->setTable("expenses");
    db.exp->setFilter("date_created = '"%ui->dateEdit_expenses->date().toString("yyyy/MM/dd")%"' AND archived = '0' ");
    db.exp->select();
    setupExpensesTable();
}

/*
 * Filter invoices by client
 */
void Dashboard::on_comboBox_inClients_activated(const QString &arg1)
{
    db.invoices = new QSqlTableModel(this, db.db);
    db.invoices->setTable("invoices");
    db.invoices->setFilter("clientname = '"%arg1%"' AND archived = '0' ");
    db.invoices->select();
    setupInvoicesTable();
}

/*
 * Filter invoices by case
 */
void Dashboard::on_comboBox_inCases_activated(const QString &arg1)
{
    db.invoices = new QSqlTableModel(this, db.db);
    db.invoices->setTable("invoices");
    db.invoices->setFilter("casename = '"%arg1%"' AND archived = '0' ");
    db.invoices->select();
    setupInvoicesTable();
}

/*
 * New invoice
 */
void Dashboard::on_pushButton_newInvoice_clicked()
{
    NewInvoice *nInvoice = new NewInvoice();
    nInvoice->show();
}

/*
 * Filter invoices by date
 */
void Dashboard::on_pushButton_invDateFilter_clicked()
{
    db.invoices = new QSqlTableModel(this, db.db);
    db.invoices->setTable("invoices");
    db.invoices->setFilter("date_created = '"%ui->dateEdit_inv->date().toString("yyyy/MM/dd")%"' AND archived = '0' ");
    db.invoices->select();
    setupInvoicesTable();
}

void Dashboard::on_pushButton_newPayment_clicked()
{
    NewPayment *nPayment = new NewPayment();
    nPayment->show();
}

/*
 * Search trust funds on return pressed
 */
void Dashboard::on_lineEdit_searchTF_returnPressed()
{
    searchTrustFunds(ui->lineEdit_searchTF->text());
}

/*
 * Search trust funds on search button pressed
 */
void Dashboard::on_pushButton_searchTF_clicked()
{
    searchTrustFunds(ui->lineEdit_searchTF->text());
}

/*
 * Search function
 */
void Dashboard::searchTrustFunds(QString clientname)
{
    trustmodel->setQuery("SELECT DISTINCT clientname FROM trust_transactions WHERE clientname LIKE %"%clientname%
                     "% AND archived = '0' ", db.db);
    setupTrustFundsTable();
}

/*
 * Withdraw from trust
 */
void Dashboard::on_pushButton_withdrawTF_clicked()
{
    TrustTransaction *tTransaction = new TrustTransaction();
    tTransaction->setupWithdrawal();
    tTransaction->show();
}

/*
 * Deposit into trust
 */
void Dashboard::on_pushButton_depositTF_clicked()
{
    TrustTransaction *tTransaction = new TrustTransaction();
    tTransaction->setupDeposit();
    tTransaction->show();
}

/*
 * Refresh trust funds
 */
void Dashboard::on_pushButton_refreshTF_clicked()
{
    trustmodel->setQuery("SELECT DISTINCT clientname FROM trust_transactions WHERE archived = '0' ", db.db);
    setupTrustFundsTable();

    db.tts->setTable("trust_transactions");
    db.tts->setFilter("archived = '0' ");
    db.tts->select();
    setupTrustTransactionsTable();
}

/*
 * Filter by date created
 */
void Dashboard::on_pushButton_dateFilterTF_clicked()
{
    db.tts->setTable("trust_transactions");
    db.tts->setFilter("date = '"%ui->dateEdit_dateTF->date().toString("yyyy-MM-dd")%"' AND archived = '0' ");
    db.tts->select();
    setupTrustTransactionsTable();
}

/*
 * Get selected client credit, debit and balance
 */
void Dashboard::getClientTrustDetails(QString client)
{
    QSqlQuery query(db.db);
    //deposit amount
    int deposits = 0;
    query.exec("SELECT SUM(amount) FROM trust_transactions WHERE clientname = '"%client%"' AND type = 'Deposit' "
                                                                                        "AND archived = '0' ");
    while (query.next()) {
        deposits = query.value(0).toInt();
    }
    ui->label_credit->setText(QString::number(deposits));
    //withdrawal amount
    int withdrawals = 0;
    query.exec("SELECT SUM(amount) FROM trust_transactions WHERE clientname = '"%client%"' AND type = 'Withdrawal' "
                                                                                        "AND archived = '0' ");
    while (query.next()) {
        withdrawals = query.value(0).toInt();
    }
    ui->label_debit->setText(QString::number(withdrawals));
    //balance
    int balance = deposits - withdrawals;
    ui->label_balance->setText(QString::number(balance));
}

/*
 * Show client trust details
 */
void Dashboard::on_tableView_trustFunds_clicked(const QModelIndex &index)
{
    QString client = trustmodel->index(index.row(), 0).data().toString();
    getClientTrustDetails(client);

    db.tts->setTable("trust_transactions");
    db.tts->setFilter("clientname = '"%client%"' AND archived = '0' ");
    db.tts->select();
    setupTrustTransactionsTable();
}

/*
 * Refresh custody items
 */
void Dashboard::on_pushButton_ciRefresh_clicked()
{
    db.citems->setTable("custody_items");
    db.citems->setFilter("archived = '0' ");
    db.citems->select();
    setupCustodyItemsTable();
}

/*
 * Custody items client filter
 */
void Dashboard::on_pushButton_ciClientFilter_clicked()
{
    QString client = ui->comboBox_ciClientFilter->currentText();
    db.citems->setTable("custody_items");
    db.citems->setFilter("clientname = '"%client%"' AND archived = '0' ");
    db.citems->select();
    setupCustodyItemsTable();
}

/*
 * New custody item
 */
void Dashboard::on_pushButton_newCI_clicked()
{
    NewCustodyItem *nci = new NewCustodyItem();
    nci->show();
}

/*
 * Checkout item
 */
void Dashboard::on_pushButton_ciCheckout_clicked()
{
    //
}
