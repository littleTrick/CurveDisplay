#include <errno.h>
#include <iostream>
#include <cstdio>
#include <stdio.h>
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
            std::cout << "the number from serial is : " << read_number << std::endl;
            std::cout << "reading from serial: ";
            for(int i = 0; i < read_number; ++i)
            {
                frame_data_from_server_.push_back(buff[i]);
                printf("%02X ", (unsigned char)buff[i]);
            }
            std::cout << std::endl;
            ParseFrame();
        }
    }

    std::cout << "data process thread exited" << std::endl;
}

void DataCollector::ParseFrame()
{
    while (frame_data_from_server_.size() >= kMinFrameLen)
    {
        if((frame_data_from_server_[0] != ADDR_LOW) || (frame_data_from_server_[1] != ADDR_HIGH))
        {
            // invalid head, clear all data
            // TODO clear until next head ?
            // TODO log
            frame_data_from_server_.clear();
            break;
        }

        frame_current_num_ = (unsigned int)frame_data_from_server_[4] << 8;
        frame_current_num_ += (unsigned char)frame_data_from_server_[3];
        if(frame_current_num_ + kHeadLen > frame_data_from_server_.size())
        {
            break;
        }

        uint16_t sum = (uint16_t)frame_data_from_server_[frame_current_num_+6] << 8;
        sum +=  (unsigned char)frame_data_from_server_[frame_current_num_+5];
        if(sum == (uint16_t)accumulate(&frame_data_from_server_[0], frame_current_num_ + 5))
        {
            Process();
        }
        else
        {
            // TODO log
        }
        QVector<char>::iterator it = frame_data_from_server_.begin();
        frame_data_from_server_.erase(it, it + frame_current_num_ + kHeadLen);
    }
}



void DataCollector::DeleteFrame(QVector<char>::iterator begin,QVector<char>::iterator end)
{
    //删除vector中所有报文
    frame_data_from_server_.erase(begin,end);
}

void DataCollector::Process()
{
    switch(frame_data_from_server_[2])                  //功能码
    {
        case ESTABLISH_CONNECTION:                      //接收到建立连接的报文
            SendFrameToServer(ESTABLISH_CONNECTION);
            break;
        case END_SAMPLING:                              //接收到采样结束的报文
            SendFrameToServer(OBTAIN_DATA);             //发送获取数据报文
            break;
        case CONFIRM_ESTABLISH_CONNECTION:              //确认链接建立
            SendFrameToServer(RESPOND_LINK);            //链路连通帧
            break;
        case SEND_DATA:                                 //收到携带数据的报文
            SaveData();                                 //存储报文
            SendFrameToServer(RESPOND_LINK);            //回复链路正常报文
            break;
        case CONFIRM_LINK:                              //测试链路
            SendFrameToServer(RESPOND_LINK);             //链路连通帧
            break;
        case STOP_DATA:
            SendDataToController();
            break;
        default:
            std::cerr << "this is a invalid frame, and the ID is:"
                      << frame_data_from_server_[2] << std::endl;
            break;
    }
}

void DataCollector::SendSetFrameToServer(QStringList curve_list,uint8_t curve_start,uint8_t curve_end,
                                          uint32_t range_start,uint32_t range_end)
{
    for(int i = 0; i < curve_list.count(); ++i)
    {
        if(curve_list.at(i) == "curve1")
        {
            _curve_chosen_map[1] = "curve1";
        }
        else if(curve_list.at(i) == "curve2")
        {
            _curve_chosen_map[2] = "curve2";
        }
        else if(curve_list.at(i) == "curve3")
        {
            _curve_chosen_map[3] = "curve3";
        }
        else if(curve_list.at(i) == "curve4")
        {
            _curve_chosen_map[4] = "curve4";
        }
        else if(curve_list.at(i) == "curve5")
        {
            _curve_chosen_map[5] = "curve5";
        }
        else if(curve_list.at(i) == "curve6")
        {
            _curve_chosen_map[6] = "curve6";
        }
    }

    unsigned char *p_range_start = (unsigned char*)&range_start;
    unsigned char *p_range_end = (unsigned char*)&range_end;

    size_t i = frame_data_to_server.size();

    frame_data_to_server_.push_back(0XA5);//0
    frame_data_to_server_.push_back(0XA8);
    frame_data_to_server_.push_back(0XA1);//2
    frame_data_to_server_.push_back(0X0A);
    frame_data_to_server_.push_back(0X00);//4
    frame_data_to_server_.push_back(*(p_range_start));
    frame_data_to_server_.push_back(*(p_range_start +1));//6
    frame_data_to_server_.push_back(*(p_range_start +2));
    frame_data_to_server_.push_back(*(p_range_start +3));//8
    frame_data_to_server_.push_back(*(p_range_end));
    frame_data_to_server_.push_back(*(p_range_end +1));//10
    frame_data_to_server_.push_back(*(p_range_end +2));
    frame_data_to_server_.push_back(*(p_range_end +3));//12
    frame_data_to_server_.push_back(curve_start);
    frame_data_to_server_.push_back(curve_end);//14

    size_t size = frame_data_to_server.size();
    uint16_t sum = accumulate(&frame_data_to_server[i], size-i);

    frame_data_to_server_.push_back(sum & 0X00FF);
    frame_data_to_server_.push_back((sum & 0XFF00) >> 8);//16

    WriteToSerial();
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
             printf("%02X ",(unsigned char)frame_data_to_server_[i]);
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

uint16_t DataCollector::accumulate(const char *data, size_t size)
{
    uint16_t sum = 0;
    for(int i = 0; i < size; i++) 
    {
        sum += (unsigned char)data[i];
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
        // this should never happen
        assert(0);
        break;
    }
    emit DataProcessStart();
}
