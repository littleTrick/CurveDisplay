#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "curve_property.h"
#include "communicationproperty.h"
#include "about.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_zoomer.h>
#include <QMainWindow>
#include <QColor>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void InitCurve();
    void DrawCurve(const QString &curve, const QVector<double> &data);

public slots:
    void OpenCommunicationCfg();
    void OpenCurveCfg();
    void OpenAbout();

    void SetAxisValue(double , double ,double ,double );
    void SetCurveItem(QStringList );
    void SetCurveColour(QString , QColor );
protected:
    void closeEvent(QCloseEvent *);

signals:
    void StartCollectData();
    void closed();
    void SerialAttribChanged(QString serial_name, unsigned int baud_rate);
//    void ComChanged(QString);
//    void BaudRateChanged(unsigned int);

private:
    QwtPlotCurve *GetCurve(const QString &curve_name);

private:
    const static int kMaxGroup = 48;
    Ui::MainWindow *ui;
    CommunicationProperty communication_property_;
    CurveProperty curve_property_;
    About about_;

    //about qwt plot
    QwtPlot *myPlot_;
    QwtPlotCurve *curve_;
    QwtPlotGrid *grid_;
    QwtPlotZoomer *zoomer_;

    QString curve_name_;
    QStringList curve_list_;
    QMap<QString, QwtPlotCurve*> curves_;
};

#endif // MAINWINDOW_H
