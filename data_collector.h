#ifndef DATA_COLLECTOR_H
#define DATA_COLLECTOR_H

#include <QVector>
#include <QObject>
#include <QTimer>
#include <QMutex>
#include <QStringList>
#include "inttypes.h"
#include "serialport.h"

class DataCollector: public QObject
{
    Q_OBJECT

public:
    DataCollector();

    bool Init();

    void ReadData();
    void SendFrameToServer(const char);
    void OverseeSrialport();
    void Stop();
    bool IsRunning() const;

    void SendSetFrametoServer(QStringList curve_list,uint8_t curve_start,uint8_t curve_end,
                                              uint32_t range_start,uint32_t range_end);
public slots:
    void SetSerialAttrib(QString serial_name, unsigned int baud_rate);

signals:
    void DataToThread(QVector<double> &);
    void DataProcessStart();

private:
    void ParseFrame();
    void JudgeFrameType();
    void DeleteFrame(QVector<char>::iterator begin,
                     QVector<char>::iterator end);
    void SaveData();
    void SendDataToController();
    int  CheckSum();
    void WriteToSerial();

    uint16_t CaculateCheckSumToServer(QVector<char>::iterator begin,QVector<char>::iterator end);

private:
    const static char ESTABLISH_CONNECTION = 0XA5;
    const static char CONFIRM_ESTABLISH_CONNECTION = 0XA1;
    const static char BEGIN_SAMPLING = 0XA7;
    const static char END_SAMPLING = 0XA6;
    const static char OBTAIN_DATA = 0XA0;
    const static char CONFIRM_LINK = 0XA8;
    const static char RESPOND_LINK = 0X55;
    const static char SEND_DATA = 0XA2;
    const static char STOP_DATA = 0XA3;
    const static char ADDR_LOW = 0XA5;
    const static char ADDR_HIGH = 0XA8;
    const static int FRAME_LENTH_MIN = 8;
    const static unsigned int kMaxRecieveLenth_ = 256;

    mutable QMutex lock_;
    bool running_;
    SerialPort serial_port_;
    QString serial_name_;
    unsigned int baud_rate_;
    int frame_current_num_;
    QVector<char> frame_data_from_server_;
    QVector<char> frame_data_to_server_;
    QVector<double> data_;
};

#endif // DATA_COLLECTOR_H
