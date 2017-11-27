#include <errno.h>
#include <iostream>
#include <cstdio>
#include "data_collector.h"

DataCollector::DataCollector():
    running_(true),
    //serial_port_("/dev/ttyUSB0"),
    frame_current_num_(0)
{
}

bool DataCollector::Init()
{
    serial_port_.SetSerialPortName(serial_name_);
    serial_port_.SetSpeed(baud_rate_);

    std::cout << "=================================" << std::endl;
    std::cout << "serial name: " << serial_name_.toStdString()
              << "  buad rate: " << baud_rate_ << std::endl;
    std::cout << "=================================" << std::endl;

    if(!serial_port_.Open())
    {
        std::cout << "open failed" << std::endl;
        return false;
    }
    return true;
}

void DataCollector::OverseeSrialport()
{
    char buff[kMaxRecieveLenth_];
    ssize_t read_number = 0;
    std::cout << "while begin before " << std::endl;

    while(IsRunning())
    {       
        read_number = serial_port_.Read(buff,sizeof(buff));
        if(read_number < 0)
        {
#ifndef NDEBUG
            std::cerr << "read error,and error number is: " << errno <<  std::endl;
#endif
            break;
        }
        else if(read_number == 0)
        {
#ifndef NDEBUG
            std::cerr << "read timeout" << std::endl;
#endif
        }
        else if(read_number > 0)
        {
            //std::cout << "the number from serial is : " << read_number << std::endl;
            //std::cout << "reading from serial: ";
            for(int i = 0; i < read_number; ++i)
            {
                frame_data_from_server_.push_back(buff[i]);
                //printf("%02X ",buff[i]);
            }
            //std::cout << std::endl;
            memset(buff,0,sizeof(buff));
            ParseFrame();
        }
    }

    std::cout << "data process thread exited" << std::endl;
}

void DataCollector::ParseFrame()
{
    if(frame_data_from_server_.size() < 8)
    {
        return;
    }

    if((frame_data_from_server_[0] != ADDR_LOW) || (frame_data_from_server_[1] != ADDR_HIGH))
    {
        //删除当前所存的无效报文
        //DeleteFrame(frame_data_from_server_.begin(),frame_data_from_server_.begin() + 1);
        DeleteFrame(frame_data_from_server_.begin(),frame_data_from_server_.end());
        return;
    }

    frame_current_num_ = ((unsigned int)frame_data_from_server_[4] << 8) + frame_data_from_server_[3];
    if(frame_current_num_ + 7 > frame_data_from_server_.size())
    {
        //数据还未收完，返回继续收
        return;
    }
    else
    {
        int sum = ((unsigned int)(unsigned char)frame_data_from_server_[frame_current_num_+6] << 8)
                + (unsigned char)frame_data_from_server_[frame_current_num_+5];
        if(sum == CheckSum())
        {
            JudgeFrameType();


            std::cout << "the number from serial is : " << (frame_current_num_ + 7) << std::endl;
            std::cout << "reading from serial: ";
            for(int i = 0; i < (frame_current_num_ + 7); ++i)
            {
                printf("%02X ",(unsigned char)frame_data_from_server_[i]);
            }
            std::cout << std::endl;


            DeleteFrame(frame_data_from_server_.begin(),frame_data_from_server_.begin()+ frame_current_num_ + 7);
        }
        else
        {
            DeleteFrame(frame_data_from_server_.begin(),frame_data_from_server_.begin() + frame_current_num_ + 7);
        }
    }
}



void DataCollector::DeleteFrame(QVector<char>::iterator begin,QVector<char>::iterator end)
{
    //删除vector中所有报文
    frame_data_from_server_.erase(begin,end);
}

void DataCollector::JudgeFrameType()
{
    //std::cout << "ID :"<< frame_data_from_server_[2] << std::endl;
    switch(frame_data_from_server_[2])//功能码
    {
        case ESTABLISH_CONNECTION:
            //std::cout << __LINE__ << std::endl;
            SendFrameToServer(ESTABLISH_CONNECTION);
            break;
        case CONFIRM_ESTABLISH_CONNECTION: //确认链接建立
            SendFrameToServer(RESPOND_LINK);    //链路连通帧
            //timer_->stop();
            break;
        case CONFIRM_LINK:                 //测试链路
            SendFrameToServer(RESPOND_LINK);    //链路连通帧
            break;
        case END_SAMPLING://采样结束
            SendFrameToServer(OBTAIN_DATA);//发送获取数据报文
            break;
        case SEND_DATA:
            SaveData();
            break;
        case STOP_DATA:
            SendDataToController();
            break;
        default:
            std::cerr << "this is a invalid frame, and the ID is:"
                      << frame_data_from_server_[2] << std::endl;
    }

}

void DataCollector::SendFrameToServer(const char id)
{
    frame_data_to_server_.push_back(0XA5);
    frame_data_to_server_.push_back(0XA8);
    frame_data_to_server_.push_back(id);
    frame_data_to_server_.push_back(0X01);
    frame_data_to_server_.push_back(0X00);
    frame_data_to_server_.push_back(0XA5);
    unsigned short sum = 0X0A5 + 0X0A8 + (unsigned char)id + 0X01 + 0X0A5;
    frame_data_to_server_.push_back(sum & 0X00FF);
    frame_data_to_server_.push_back((sum & 0XFF00) >> 8);

    WriteToSerial();
}

void DataCollector::WriteToSerial()
{
    const char *buff = &frame_data_to_server_[0];
    ssize_t nwrite = serial_port_.Write(buff,frame_data_to_server_.size());
    if(nwrite == -1)
    {
#ifndef NDEBUG
        std::cerr << "write to serial is wrong ! and the error number is: " << errno << std::endl;
#endif
        return;
    }
    else if(nwrite > 0)
    {
        std::cout << "the number to serial is : " << nwrite << std::endl;
        std::cout << "writing to serial: ";
        for(int i = 0; i < nwrite; ++i)
        {
             printf("%02X",(unsigned char)frame_data_to_server_[i]);
        }
        std::cout << std::endl;

        //DeleteFrame(frame_data_to_server_.begin(),frame_data_to_server_.begin() + nwrite);
        frame_data_to_server_.erase(frame_data_to_server_.begin(),frame_data_to_server_.begin() + nwrite);
    }
}

void DataCollector::SaveData()
{
    for(int i = 0; i < frame_current_num_; ++i)
    {
        data_.push_back(frame_data_from_server_[5 + i]);
    }

}

void DataCollector::SendDataToController()
{
    emit DataToThread(data_);
}

int DataCollector::CheckSum()
{
    int sum = 0;
    for(int i = 0; i < frame_current_num_ + 5; ++i) //5为报文头长度
    {
        sum += (unsigned char)frame_data_from_server_[i];
    }
    return sum;
}

void DataCollector::Stop()
{
    lock_.lock();
    running_ = false;
    lock_.unlock();
}

bool DataCollector::IsRunning() const
{
    lock_.lock();
    bool r = running_;
    lock_.unlock();
    return r;
}

void DataCollector::SetSerialAttrib(QString serial_name, unsigned int baud_rate)
{
    serial_name_ = serial_name;

    switch (baud_rate)
    {
    case 115200:
        baud_rate_ = B115200;
        break;
    case 9600:
        baud_rate_ = B9600;
        break;
    default:
        break;
    }

    emit DataProcessStart();
}
