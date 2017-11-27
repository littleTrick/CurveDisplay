#ifndef COMMUNICATION_PROPERTY_H
#define COMMUNICATION_PROPERTY_H

#include <QDialog>

namespace Ui {
class CommunicationProperty;
}

class CommunicationProperty: public QDialog
{
    Q_OBJECT
    
public:
    explicit CommunicationProperty(QWidget *parent = 0);
    ~CommunicationProperty();

signals:
    void SerialAttribChanged(QString serial_name, unsigned int baud_rate);
//    void ComChanged(QString);
//    void BaudRateChanged(unsigned int);


public slots:
    void TriggeredBtnSaveCfg();
    void TriggeredBtnCancelCfg();
    
private:
    Ui::CommunicationProperty*ui;
};

#endif // COMMUNICATION_PROPERTY_H
