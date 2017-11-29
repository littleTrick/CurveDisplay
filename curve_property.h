#ifndef CURVE_PROPERTY_H
#define CURVE_PROPERTY_H

#include <QDialog>
#include <QStringList>
#include <QListWidgetItem>
#include <map>
#include "inttypes.h"

namespace Ui {
class CurveProperty;
}

class CurveProperty : public QDialog
{
    Q_OBJECT
    
public:
    explicit CurveProperty(QWidget *parent = 0);
    ~CurveProperty();
    QPalette GetPalette()
    {
        return palette_;
    }

public slots:
    void ChooseColour(QListWidgetItem *);
    void ChooseBtnFinish();
    void ChooseBtnXYSet();
    void AddCurveItem();
    void DelCurveItem();

signals:
    void setXYAxis(double ,double ,double ,double );
    void setCurveItem(QStringList ,uint8_t,uint8_t,uint32_t,uint32_t);
    void CurveColourChanged(QString ,QColor );

private:
    void Init();
private:
    Ui::CurveProperty *ui;
    QPalette palette_;
    QStringList selects_item_;
    std::map<QString,int> _curve;
    QVector<int> _selected_number;
};

#endif // CURVE_PROPERTY_H
