#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include "dbfunctions.h"

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = 0);
    ~LoginWindow();

private slots:
    void on_lineEdit_pass_returnPressed();

    void on_pushButton_login_clicked();

private:
    Ui::LoginWindow *ui;

    DBfunctions db;

    void login();
};

#endif // LOGINWINDOW_H
