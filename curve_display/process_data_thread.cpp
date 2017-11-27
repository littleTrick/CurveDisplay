#include "process_data_thread.h"
#include <iostream>

ProcessDataThread::ProcessDataThread()
{
    connect(&data_collector_,SIGNAL(DataToThread(QVector<double>&)),
            this,SLOT(DataToDraw(QVector<double>&)));
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

void ProcessDataThread::DataToDraw(QVector<double> &data)
{
    emit DataToControllor(data);
}

void ProcessDataThread::SendFrameToServer(const char id)
{
    data_collector_.SendFrameToServer(id);
}

void ProcessDataThread::Stop()
{
    data_collector_.Stop();
}