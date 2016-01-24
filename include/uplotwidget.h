#ifndef UPLOTWIDGET_H
#define UPLOTWIDGET_H

#include <QWidget>
#include "qcustomplot.h"
#include "common.h"

class UPlotWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UPlotWidget(QWidget *parent = 0);
    ~UPlotWidget();

    void set_plot(double x_axis_min, double x_axis_max, std::string camera_type, std::string task_type);
    void set_data(double key, double val1, double val2=0, double val3=0);
    void clear_plot();
    void clear_graph_data();

protected:
    QCustomPlot *plot_;

    std::string curr_camera_type_;
    std::string curr_task_type_;

    double x_axis_max_;
    double x_axis_min_;
    double y_axis_max_;
    double y_axis_min_;

signals:

public slots:
};

#endif // UPLOTWIDGET_H
