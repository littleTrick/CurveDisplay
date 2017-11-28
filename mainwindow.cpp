#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qwt_legend.h>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _curve_start(0),
    _curve_end(0),
    _range_start(0),
    _range_end(0)
{
    ui->setupUi(this);
    connect(ui->communication_property_menu,SIGNAL(triggered()),
            this,SLOT(OpenCommunicationCfg()));
    connect(ui->curve_property_menu,SIGNAL(triggered()),
            this,SLOT(OpenCurveCfg()));
    connect(ui->about_software,SIGNAL(triggered()),
            this,SLOT(OpenAbout()));
    connect(&curve_property_,SIGNAL(setXYAxis(double ,double ,double ,double )),
            this,SLOT(SetAxisValue(double , double ,double ,double )));
    connect(&curve_property_,SIGNAL(setCurveItem(QStringList,uint8_t,uint8_t,uint32_t,uint32_t)),
            this,SLOT(SetCurveItem(QStringList,uint8_t,uint8_t,uint32_t,uint32_t)));
    connect(&curve_property_,SIGNAL(CurveColourChanged(QString,QColor)),
            this,SLOT(SetCurveColour(QString, QColor)));
    connect(ui->btn_data_collection,SIGNAL(clicked()),
            this,SLOT(CollectData()));
    connect(&communication_property_,SIGNAL(SerialAttribChanged(QString, unsigned int)),
            this,SIGNAL(SerialAttribChanged(QString, unsigned int)));
//    connect(&communication_property_,SIGNAL(ComChanged(QSrting)),
//            this,SIGNAL(ComChanged(QString)));
//    connect(&communication_property_,SIGNAL(BaudRateChanged(uint)),
//            this,SIGNAL(BaudRateChanged(uint)));
    //about qwt plot
    InitCurve();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OpenCommunicationCfg()
{
    communication_property_.setVisible(true);

}

void MainWindow::OpenCurveCfg()
{
    curve_property_.setVisible(true);
}

void MainWindow::OpenAbout()
{
    about_.setVisible(true);
}

void MainWindow::InitCurve()
{
    myPlot_ = new QwtPlot(this);
    myPlot_->setGeometry(50,50,600,400);
    curve_name_ = "mechanical characteristics curve";
    myPlot_->setTitle(curve_name_);
    myPlot_->setCanvasBackground(Qt::gray);

    myPlot_->setAxisTitle(QwtPlot::xBottom, "x value, time");
    myPlot_->setAxisTitle(QwtPlot::yLeft,"y value");
    myPlot_->setAxisScale(QwtPlot::yLeft,0,4,0.5);
    myPlot_->setAxisScale(QwtPlot::xBottom, 0,4,0.5);

    grid_ = new QwtPlotGrid();
    grid_->enableX(true);
    grid_->enableY(true);
    grid_->setMajorPen(Qt::black,0,Qt::DotLine);
    grid_->attach(myPlot_);

    zoomer_ = new QwtPlotZoomer(myPlot_->canvas());
    zoomer_->setRubberBandPen(QColor(Qt::blue));
    zoomer_->setMousePattern(QwtEventPattern::MouseSelect2,Qt::RightButton,Qt::ControlModifier);
    zoomer_->setMousePattern(QwtEventPattern::MouseSelect3,Qt::RightButton);

    myPlot_->show();
}

void MainWindow::SetAxisValue(double x_min,double x_max,double y_min,double y_max)
{
    myPlot_->setAxisScale(QwtPlot::yLeft,y_min,y_max,0.5);
    myPlot_->setAxisScale(QwtPlot::xBottom, x_min,x_max,0.5);
    myPlot_->replot();
}

void MainWindow::SetCurveItem(QStringList curve_list,uint8_t curve_start,uint8_t curve_end,
                              uint32_t range_start,uint32_t range_end)
{
    curve_list_ = curve_list;
    _curve_start = curve_start;
    _curve_end = curve_end;
    _range_start = range_start;
    _range_end = range_end;
}

void MainWindow::DrawCurve(const QString &curve, const QVector<double> &data)
{
    if(curve_list_.contains(curve))
    {
        QVector<double> x;
        for(int i= 0; i < data.size(); ++i)
        {
            x.push_back(i);
        }
        GetCurve(curve)->setSamples(x,data);
        myPlot_->replot();
    }
}

void MainWindow::SetCurveColour(QString curve_name, QColor color)
{
    GetCurve(curve_name)->setPen(color,1);
}

QwtPlotCurve *MainWindow::GetCurve(const QString &curve_name)
{
    if (!curves_.contains(curve_name))  {
        curves_[curve_name] = new QwtPlotCurve(curve_name);
        curves_[curve_name]->setTitle(curve_name);
        curves_[curve_name]->setPen(Qt::red,1);
        curves_[curve_name]->attach(myPlot_);
        myPlot_->insertLegend(new QwtLegend(),QwtPlot::RightLegend);
    }
    return curves_[curve_name];
}

void MainWindow::closeEvent(QCloseEvent *)
{
    emit closed();
}

void MainWindow::CollectData()
{
    emit StartCollectData(curve_list_,_curve_start,_curve_end,_range_start,_range_end);
}
