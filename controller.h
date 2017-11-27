#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include "login.h"
#include "mainwindow.h"
#include "process_data_thread.h"

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = 0);
    ~Controller();

    void start();

signals:
    
private slots:
    void StartCollectData();
    void DataReadyDraw(QVector<double> &);
    void OnMainWindowClosed();
    void ProcessStart();

private:
    Login login_;
    MainWindow main_window_;
    ProcessDataThread process_data_thread_;
    
};

#endif // CONTROLLER_H
