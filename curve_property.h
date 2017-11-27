#ifndef CURVE_PROPERTY_H
#define CURVE_PROPERTY_H

#include <QDialog>
#include <QStringList>
#include <QListWidgetItem>

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
    void setCurveItem(QStringList );
    void CurveColourChanged(QString ,QColor );

private:
    Ui::CurveProperty *ui;
    QPalette palette_;
    QStringList selects_item_;
};

#endif // CURVE_PROPERTY_H
