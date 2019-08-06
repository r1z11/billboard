#ifndef NEWCLIENT_H
#define NEWCLIENT_H

#include <QDialog>
#include <QFileDialog>
#include <dbfunctions.h>

namespace Ui {
class newclient;
}

class newclient : public QDialog
{
    Q_OBJECT

public:
    explicit newclient(QWidget *parent = 0);
    ~newclient();

private slots:
    void on_pushButton_cancel_clicked();

    void on_pushButton_addClient_clicked();

    void on_checkBox_co_clicked(bool checked);
    
    void on_pushButton_browse_clicked();

    void on_checkBox_sameInfoAsClient_toggled(bool checked);

private:
    Ui::newclient *ui;

    DBfunctions db;
    QDateTime today;
};

#endif // NEWCLIENT_H
