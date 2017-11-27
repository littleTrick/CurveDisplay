#include "communicationproperty.h"
#include "ui_communicationproperty.h"

CommunicationProperty::CommunicationProperty(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CommunicationProperty)
{
    ui->setupUi(this);
}

CommunicationProperty::~CommunicationProperty()
{
    delete ui;
}
