#include "curve_property.h"
#include "ui_curve_property.h"
#include <cassert>
#include <QDebug>
#include <QColor>
#include <QColorDialog>
#include <algorithm>
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
    Init();
}

CurveProperty::~CurveProperty()
{
    delete ui;
}

void CurveProperty::Init()
{
    _curve.insert(make_pair("curve1",1));
    _curve.insert(make_pair("curve2",2));
    _curve.insert(make_pair("curve3",3));
    _curve.insert(make_pair("curve4",4));
    _curve.insert(make_pair("curve5",5));
    _curve.insert(make_pair("curve6",6));

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
    QVector<int> selected_number;
    qDebug()<< list_number;
    for (int i = 0; i < list_number; ++i)
    {
        QString current_item = ui->list_selected_item->item(i)->text();
        selects_item_ << current_item;
         //selects_item_ << ui->list_selected_item->item(i)->text();
        selected_number.push_back(_curve[current_item]);
    }
    sort(selected_number.begin(),selected_number.end());
    uint8_t selected_number_begin = static_cast<char> (*(selected_number.begin()));
    uint8_t selected_number_end = static_cast<char> (*(selected_number.end()-1));

    uint32_t range_begin = ui->start_range->text().toUInt();
    uint32_t range_end = ui->end_range->text().toUInt();

//    std::cout << "range_begin: "<< range_begin << "  range_end: " << range_end << std::endl;
//    printf("a=%02X ", range_begin);
//    printf("b=%02X\n", range_end);
//    for(QVector<int>::Iterator it = selected_number.begin();it != selected_number.end();++it)
//    {
//        std::cout << *it << " ";
//    }
//    std::cout << std::endl;
    emit setCurveItem(selects_item_,selected_number_begin,selected_number_end,range_begin,range_end);
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
