#ifndef COMMUNICATIONPROPERTY_H
#define COMMUNICATIONPROPERTY_H

#include <QDialog>

namespace Ui {
class CommunicationProperty;
}

class CommunicationProperty : public QDialog
{
    Q_OBJECT
    
public:
    explicit CommunicationProperty(QWidget *parent = 0);
    ~CommunicationProperty();
    
private:
    Ui::CommunicationProperty *ui;
};

#endif // COMMUNICATIONPROPERTY_H
