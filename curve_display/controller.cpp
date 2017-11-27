#include "controller.h"
#include <QVector>
#include <iostream>

Controller::Controller(QObject *parent) :
    QObject(parent)
{
    connect(&main_window_,SIGNAL(StartCollectData()),this,SLOT(StartCollectData()));
    connect(&process_data_thread_,SIGNAL(DataToControllor(QVector<double>&)),
            this,SLOT(DataReadyDraw(QVector<double>&)));
    connect(&main_window_,SIGNAL(closed()),this,SLOT(OnMainWindowClosed()));
    connect(&main_window_,SIGNAL(SerialAttribChanged(QString, unsigned int)),
            &process_data_thread_,SIGNAL(SerialAttribChanged(QString, unsigned int)));
//    connect(&main_window_,SIGNAL(ComChanged(QString)),
//            &process_data_thread_,SLOT(SetComName(QString)));
//    connect(&main_window_,SIGNAL(BaudRateChanged(uint)),
//            &process_data_thread_,SLOT(SetBaudRate(uint)));
    connect(&process_data_thread_,SIGNAL(DataProcessStart()),
            this,SLOT(ProcessStart()));
}

Controller::~Controller()
{
    process_data_thread_.wait();
}

void Controller::start()
{
    if(login_.exec() != QDialog::Accepted)
    {
        return;
    }

    main_window_.show();
    //process_data_thread_.start();
}

void Controller::StartCollectData()
{
    process_data_thread_.SendFrameToServer(0XA7);//数据采集开始报文
}

void Controller::DataReadyDraw(QVector<double> &data)
{
    main_window_.DrawCurve("curve1",data);
}

void Controller::OnMainWindowClosed()
{
    std::cout << "main window closed" << std::endl;
    process_data_thread_.Stop();
}

void Controller::ProcessStart()
{
    process_data_thread_.start();
}
