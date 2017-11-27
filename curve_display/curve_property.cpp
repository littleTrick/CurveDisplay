#include "curve_property.h"
#include "ui_curve_property.h"
#include <cassert>
#include <QDebug>
#include <QColor>
#include <QColorDialog>
#include <iostream>

using namespace std;

CurveProperty::CurveProperty(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CurveProperty)
{
    ui->setupUi(this);
    connect(this,SIGNAL(finished(int)),this,SLOT(ChooseBtnFinish()));
    connect(ui->btn_axis_set,SIGNAL(clicked()),this,SLOT(ChooseBtnXYSet()));
    connect(ui->btn_add,SIGNAL(clicked()),this,SLOT(AddCurveItem()));
    connect(ui->btn_del,SIGNAL(clicked()),this,SLOT(DelCurveItem()));
    connect(ui->list_selected_item,SIGNAL(itemDoubleClicked(QListWidgetItem* )),
            this,SLOT(ChooseColour(QListWidgetItem* )));
}

CurveProperty::~CurveProperty()
{
    delete ui;
}

void CurveProperty::ChooseColour(QListWidgetItem *currentItem)
{
    QColor color = QColorDialog::getColor(QColor::fromRgb(255,255,0));
    currentItem->setTextColor(color);
    emit CurveColourChanged(currentItem->text(),color);
}

void CurveProperty::ChooseBtnFinish()
{
    int list_number = ui->list_selected_item->count();
    qDebug()<< list_number;
    for (int i = 0; i < list_number; ++i)
    {
         selects_item_ << ui->list_selected_item->item(i)->text();
    }
    emit setCurveItem(selects_item_);
}

void CurveProperty::ChooseBtnXYSet()
{
    emit setXYAxis(ui->x_min->value(),ui->x_max->value(),ui->y_min->value(),ui->y_max->value());
}

void CurveProperty::AddCurveItem()
{
    int row = ui->list_unselected_item->currentRow();
    if (row >= 0 && row < ui->list_unselected_item->count())
    {
        QListWidgetItem *item = ui->list_unselected_item->takeItem(row);
        ui->list_selected_item->addItem(item);
    }
}

void CurveProperty::DelCurveItem()
{
    int row = ui->list_selected_item->currentRow();
    if (row >= 0)
    {
       QListWidgetItem *item = ui->list_selected_item->takeItem(row);
       ui->list_unselected_item->addItem(item);
    }
}
