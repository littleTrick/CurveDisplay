#include <errno.h>
#include <iostream>
#include <cstdio>
#include <stdio.h>
#include "data_collector.h"

DataCollector::DataCollector():
    running_(true),
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
            //std::cerr << "read timeout" << std::endl;
#endif
        }
        else if(read_number > 0)
        {
            std::cout << "the number from serial is : " << read_number << std::endl;
            std::cout << "reading from serial: ";
            for(int i = 0; i < read_number; ++i)
            {
                unsigned char c = (unsigned char)buff[i];
                frame_data_from_server_.push_back(c);
                printf("%02X ",(unsigned char)buff[i]);
            }
            std::cout << std::endl;
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

    frame_current_num_ = ((unsigned int)frame_data_from_server_[4] << 8) + (unsigned char)frame_data_from_server_[3];
    std::cout << "frame_current_num_ : " << frame_current_num_ << std::endl;
    if(frame_current_num_ + 7 > frame_data_from_server_.size())
    {
        //数据还未收完，返回继续收
        return;
    }
    else
    {
        uint16_t sum = ((unsigned int)(unsigned char)frame_data_from_server_[frame_current_num_+6] << 8)
                + (unsigned char)frame_data_from_server_[frame_current_num_+5];
        if(sum == CheckSum())
        {
            JudgeFrameType();

//            std::cout << "the number from serial is : " << (frame_current_num_ + 7) << std::endl;
//            std::cout << "reading from serial: ";
//            for(int i = 0; i < (frame_current_num_ + 7); ++i)
//            {
//                printf("%02X ",(unsigned char)frame_data_from_server_[i]);
//            }
//            std::cout << std::endl;


            DeleteFrame(frame_data_from_server_.begin(),frame_data_from_server_.begin()+ frame_current_num_ + 7);
            std::cout << "the count of frame_data_server is :" << frame_data_from_server_.size() << std::endl;
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
            SendFrameToServer(RESPOND_LINK);
            SendDataToController();
            break;
        default:
            std::cerr << "this is a invalid frame, and the ID is:"
                      << frame_data_from_server_[2] << std::endl;
    }

}

void DataCollector::SendSetFrametoServer(QStringList curve_list,uint8_t curve_start,uint8_t curve_end,
                                          uint32_t range_start,uint32_t range_end)
{
    for(int i = 0; i < curve_list.count(); ++i)
    {
        if(curve_list.at(i) == "curve1")
        {
            _curve_chosen_map[1] = "curve1";
            _curve__data_map["curve1"] = _data_curve1;
        }
        else if(curve_list.at(i) == "curve2")
        {
            _curve_chosen_map[2] = "curve2";
            _curve__data_map["curve2"] = _data_curve2;
        }
        else if(curve_list.at(i) == "curve3")
        {
            _curve_chosen_map[3] = "curve3";
            _curve__data_map["curve3"] = _data_curve3;
        }
        else if(curve_list.at(i) == "curve4")
        {
            _curve_chosen_map[4] = "curve4";
            _curve__data_map["curve4"] = _data_curve4;
        }
        else if(curve_list.at(i) == "curve5")
        {
            _curve_chosen_map[5] = "curve5";
            _curve__data_map["curve5"] = _data_curve5;
        }
        else if(curve_list.at(i) == "curve6")
        {
            _curve_chosen_map[6] = "curve6";
            _curve__data_map["curve6"] = _data_curve6;
        }
    }

    _range_start = range_start;
    unsigned char *p_range_start = (unsigned char*)&range_start;
    unsigned char *p_range_end = (unsigned char*)&range_end;

    unsigned int  begin_location = frame_data_to_server_.size();
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

    unsigned int end_location = frame_data_to_server_.size();
    uint16_t sum = CaculateCheckSumToServer(begin_location,end_location);
    printf("begin: %02X ,end: %02X \n",begin_location,end_location);

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
    unsigned char current_curve_id = frame_data_from_server_[5];//该条报文曲线编号 输出1
    std::map<int,QString>::iterator iter_curve = _curve_chosen_map.find(current_curve_id);//查找该曲线编号会否被选择
    if(iter_curve != _curve_chosen_map.end())
    {
        QString current_curve_name = iter_curve->second;
        uint32_t data_offset = ((uint32_t)frame_data_from_server_[9] << 24) + ((uint32_t)frame_data_from_server_[8] << 16)
                                 + ((uint16_t)frame_data_from_server_[7] << 8) + ((uint8_t)frame_data_from_server_[6]); //数据偏移量
        std::map<QString,QVector<char> >::iterator iter_data = _curve__data_map.find(current_curve_name);//该曲线对应的数据缓存
        if((data_offset - _range_start) != iter_data->second.size())
        {
            return;                                             //数据发生重叠或者缺省
        }
        else
        {
            for(int i = 0; i < frame_current_num_ - 5; ++i)
            {
                iter_data->second.push_back(frame_data_from_server_[10 + i]);//10为数据位开始坐标
            }
        }
    }
    else
    {
        return;
    }
}

void DataCollector::SendDataToController()
{
    unsigned char current_curve_id = frame_data_from_server_[5];//该条报文曲线编号
    std::map<int,QString>::iterator iter_curve = _curve_chosen_map.find(current_curve_id);//查找该曲线编号会否被选择
    if(iter_curve != _curve_chosen_map.end())
    {
        QString current_curve_name = iter_curve->second;  //该条曲线名
        std::map<QString,QVector<char> >::iterator iter_data = _curve__data_map.find(current_curve_name);
        for(int i = 0; i < (iter_data->second).size(); i = i + 4)
        {
            float d=*(float*)&((iter_data->second)[i]);
            data_.push_back(d);
        }

        std::cout << "data which is in collector : " << data_.size() << std::endl;
        for(QVector<double>::const_iterator it = data_.begin();it != data_.end();++it)
        {
            std::cout << *it << " " << std::endl;
        }
        std::cout << std::endl;

        emit DataToThread(current_curve_name,data_);
    }
}

uint16_t DataCollector::CheckSum()
{
    uint16_t sum = 0;
    for(int i = 0; i < frame_current_num_ + 5; ++i) //5为报文头长度
    {
        sum += (unsigned char)frame_data_from_server_[i];
    }
    return sum;
}

uint16_t DataCollector::CaculateCheckSumToServer(unsigned int begin,unsigned int end)
{
    uint16_t sum = 0;
    for(int i = begin; i < end; ++i)
    {
        sum += (unsigned char)frame_data_to_server_[i];
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
