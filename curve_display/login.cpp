#include "login.h"
#include "ui_login.h"
#include "mainwindow.h"
#include <iostream>
#include <QMessageBox>

using namespace std;

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    connect(ui->btn_login,SIGNAL(clicked()),this,SLOT(ClickedBtnLogin()));
    connect(ui->btn_logout,SIGNAL(clicked()),this,SLOT(ClickedBtnLogout()));
    ui->user_name->setText(tr("sojo"));
    ui->password->setText(tr("123456"));
}

Login::~Login()
{
    delete ui;
}

void Login::ClickedBtnLogin()
{
    if(ui->user_name->text() == tr("sojo") && ui->password->text() == tr("123456"))
    {
        accept();
    }
    else
    {
        QMessageBox::warning(NULL,"warning","please input right username or password");
    }
}

void Login::ClickedBtnLogout()
{
    reject();
}
