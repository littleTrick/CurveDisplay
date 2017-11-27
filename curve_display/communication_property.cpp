#include "communication_property.h"
#include "ui_communication_property.h"
#include <iostream>

CommunicationProperty::CommunicationProperty(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CommunicationProperty)
{
    ui->setupUi(this);
    ui->com_name->setText(tr("/dev/ttyS1"));
    connect(ui->btn_save,SIGNAL(clicked()),this,SLOT(TriggeredBtnSaveCfg()));
    connect(ui->btn_cancle,SIGNAL(clicked()),this,SLOT(TriggeredBtnCancelCfg()));
}

CommunicationProperty::~CommunicationProperty()
{
    delete ui;
}

void CommunicationProperty::TriggeredBtnSaveCfg()
{
    QString serial_name = ui->com_name->text();
    QString baud_rate_string = ui->baud_rate->currentText();
    unsigned int baud_rate = baud_rate_string.toUInt();

    emit SerialAttribChanged(serial_name, baud_rate);
}

void CommunicationProperty::TriggeredBtnCancelCfg()
{
    //add cancel configuration
}
