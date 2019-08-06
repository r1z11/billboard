#ifndef DBFUNCTIONS
#define DBFUNCTIONS

#include <QtSql>
#include <QMessageBox>
#include <QDebug>

/*
    database functions
*/

class DBfunctions {
public:
    QSqlDatabase db;
    //user types model
    QSqlTableModel *utmodel;
    //upcoming tasks model
    QSqlTableModel *utasks;
    //calender tasks
    QSqlTableModel *ctasks;
    //tasks model
    QSqlTableModel *tasks;
    //clients model
    QSqlTableModel *clients;
    QSqlTableModel *iClients;
    //cases model
    QSqlTableModel *cases;
    //time entries
    QSqlTableModel *tentries;
    //expenses
    QSqlTableModel *exp;
    //invoices
    QSqlTableModel *invoices;
    //trust funds
    QSqlTableModel *tfunds;
    //trust transactions
    QSqlTableModel *tts;
    //custody items
    QSqlTableModel *citems;

    //create database connection
    void createConnection(QString connectionName){
        db = QSqlDatabase::addDatabase("QSQLITE", connectionName);

        QString dbPath = QDir::toNativeSeparators(qApp->applicationDirPath()+"/billboard");

        db.setDatabaseName(dbPath);
        qDebug() <<dbPath;

        db.open();
    }

    //check if connection is open
    bool isDbOpen()
    {
        if (!db.open()) {
            qDebug() << db.lastError().text();

            QMessageBox::critical(0, qApp->tr("Database error!"),
                                  qApp->tr("Unable to establish a database connection.\n"),
                                  QMessageBox::Close);

            return false;
        }else{
            return true;
        }
    }

    //get the current user logged in
    QString getCurrentUser()
    {
        QString username = NULL;
        QSqlQuery query(db);
        query.exec("SELECT username FROM users WHERE isloggedin = '1'");

        while (query.next()) {
            username = query.value(0).toString();
        }
        return username;
    }

    //get the current user's  fullname logged in
    QString getCurrentUserFullName()
    {
        QString fullname = NULL;
        QSqlQuery query(db);
        query.exec("SELECT fullname FROM users WHERE isloggedin = '1'");

        while (query.next()) {
            fullname = query.value(0).toString();
        }
        return fullname;
    }

    //set the current user logged in
    void setCurrentUser(QString username)
    {
        QSqlQuery query(db);
        query.prepare(QString("UPDATE users SET isloggedin = '1' WHERE username = :username "));
        query.bindValue(":username", username);
        query.exec();
    }

    //is the current user logged in
    bool isUserLoggedIn(QString username)
    {
        bool isloggedin = false;
        QSqlQuery query(db);
        query.prepare(QString("SELECT isloggedin FROM users WHERE username = :username "));
        query.bindValue(":username", username);
        query.exec();

        while (query.next()) {
            isloggedin = query.value(0).toBool();
        }
        return isloggedin;
    }

    //logout current user
    void logoutCurrentUser(QString username)
    {
        QSqlQuery query(db);
        query.prepare(QString("UPDATE users SET isloggedin = '0' WHERE username = :username "));
        query.bindValue(":username", username);
        query.exec();
    }

    //does user exist
    bool userExists(QString username)
    {
        bool exists = false;
        QSqlQuery query(db);
        query.prepare(QString("SELECT username FROM users WHERE username = :username "));
        query.bindValue(":username", username);
        query.exec();

        while (query.next()) {
            QString username2 = query.value(0).toString();

            if(username == username2)
                exists = true;
        }
        return exists;
    }

    //get the current user's  userid
    int getUserID(QString fullname)
    {
        int userid = 0;
        QSqlQuery query(db);;
        query.prepare(QString("SELECT userid FROM users WHERE fullname = :fullname "));
        query.bindValue(":fullname", fullname);
        query.exec();

        while (query.next()) {
            userid = query.value(0).toInt();
        }
        return userid;
    }

    //get the taskid
    int getTaskID(QString taskname)
    {
        int taskid = 0;
        QSqlQuery query(db);;
        query.prepare(QString("SELECT taskid FROM tasks WHERE taskname = :taskname "));
        query.bindValue(":taskname", taskname);
        query.exec();

        while (query.next()) {
            taskid = query.value(0).toInt();
        }
        return taskid;
    }

    //get the clientid
    int getClientID(QString clientname)
    {
        int clientid = 0;
        QSqlQuery query(db);;
        query.prepare(QString("SELECT clientid FROM clients WHERE clientname = :fullname "));
        query.bindValue(":fullname", clientname);
        query.exec();

        while (query.next()) {
            clientid = query.value(0).toInt();
        }
        return clientid;
    }

    //get the caseid
    int getCaseID(QString casename)
    {
        int caseid = 0;
        QSqlQuery query(db);;
        query.prepare(QString("SELECT caseid FROM cases WHERE casename = :fullname "));
        query.bindValue(":fullname", casename);
        query.exec();

        while (query.next()) {
            caseid = query.value(0).toInt();
        }
        return caseid;
    }

    //get all cases
    QStringList getCases()
    {
        QStringList cases;
        QSqlQuery query(db);
        query.prepare(QString("SELECT * FROM cases"));
        query.exec();

        while(query.next()){
            cases.append(query.value(0).toString());
        }
        return cases;
    }

    //delete task
    bool deleteTask(QString taskname)
    {
        QSqlQuery query(db);;
        query.prepare(QString("UPDATE tasks SET archived = '1'  WHERE taskname = :taskname "));
        query.bindValue(":taskname", taskname);

        if(query.exec()){
            return true;
        } else {
            return false;
        }
    }

};
#endif // DBFUNCTIONS

