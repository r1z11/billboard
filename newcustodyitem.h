#ifndef NEWCUSTODYITEM_H
#define NEWCUSTODYITEM_H

#include <QDialog>
#include <dbfunctions.h>

namespace Ui {
class NewCustodyItem;
}

class NewCustodyItem : public QDialog
{
    Q_OBJECT

public:
    explicit NewCustodyItem(QWidget *parent = 0);
    ~NewCustodyItem();

private slots:
    void on_pushButton_close_clicked();

private:
    Ui::NewCustodyItem *ui;

    DBfunctions db;
    QCompleter *csearchCompleter;
};

#endif // NEWCUSTODYITEM_H
