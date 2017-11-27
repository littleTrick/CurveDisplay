#include "about.h"
#include "ui_about.h"

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    connect(ui->btn_close,SIGNAL(clicked()),this,SLOT(ClickedBtnClose()));
}

About::~About()
{
    delete ui;
}

void About::ClickedBtnClose()
{
    this->close();
}
