#include "process_data_thread.h"
#include <iostream>

ProcessDataThread::ProcessDataThread()
{
    connect(&data_collector_,SIGNAL(DataToThread(const QString,const QVector<double>)),
            this,SLOT(DataToDraw(const QString,const QVector<double>)));
    connect(this,SIGNAL(SerialAttribChanged(QString, unsigned int)),
            &data_collector_,SLOT(SetSerialAttrib(QString, unsigned int)));
    connect(&data_collector_,SIGNAL(DataProcessStart()),this,SIGNAL(DataProcessStart()));
}

void ProcessDataThread::run()
{
    if (!data_collector_.Init())
    {
        data_collector_.Stop();
        return;
    }
    std::cout << "int the run " << std::endl;
    data_collector_.OverseeSrialport();
}

void ProcessDataThread::DataToDraw(const QString curve_name,const QVector<double> data)
{
    emit DataToControllor(curve_name,data);
}

void ProcessDataThread::SendFrameToServer(QStringList curve_list,uint8_t curve_start,uint8_t curve_end,
                                          uint32_t range_start,uint32_t range_end)
{
    data_collector_.SendSetFrametoServer(curve_list,curve_start,curve_end,range_start,range_end);
}

void ProcessDataThread::Stop()
{
    data_collector_.Stop();
}
