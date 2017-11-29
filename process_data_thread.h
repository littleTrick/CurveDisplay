#ifndef PROCESS_DATA_THREAD_H
#define PROCESS_DATA_THREAD_H

#include <QThread>
#include <QObject>
#include "data_collector.h"

class ProcessDataThread:public QThread
{
    Q_OBJECT

public:
    ProcessDataThread();
    virtual void run();
    void SendFrameToServer(QStringList,uint8_t,uint8_t,uint32_t,uint32_t);
    void Stop();

signals:
    void DataToControllor(QVector<double>&);
    void SerialAttribChanged(QString serial_name, unsigned int baud_rate);
    void DataProcessStart();

private slots:
    void DataToDraw(QVector<double>&);
//    void SetComName(QString);
//    void SetBaudRate(unsigned int);

private:
    DataCollector data_collector_;
};

#endif // PROCESS_DATA_THREAD_H
