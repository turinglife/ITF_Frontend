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

    void set_plot(double x_axis_min, double x_axis_max, std::string type);
    void set_data(double key, double value);
    void clear_plot();

protected:
    QCustomPlot *plot_;

    std::string curr_type_;

    double x_axis_max_;
    double x_axis_min_;
    double y_axis_max_;
    double y_axis_min_;

signals:

public slots:
};

#endif // UPLOTWIDGET_H
