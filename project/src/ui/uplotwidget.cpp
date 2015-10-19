#include "uplotwidget.h"

UPlotWidget::UPlotWidget(QWidget *parent)
    : QWidget(parent)
{
    plot_ = new QCustomPlot(this);
    //![Layout]
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(plot_);
    setLayout(layout);
}

UPlotWidget::~UPlotWidget()
{

}

void UPlotWidget::set_plot(double x_axis_min, double x_axis_max, string type)
{
    x_axis_min_ = x_axis_min;
    x_axis_max_ = x_axis_max;
    y_axis_min_ = 0;
    y_axis_max_ = 30;
    curr_type_ = type;

    //![Add Graph]
    plot_->addGraph();
    plot_->graph(0)->setName("Counting");
    plot_->graph(0)->setPen(QPen(QColor(255, 100, 0)));
    plot_->graph(0)->setLineStyle(QCPGraph::lsLine);
    plot_->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));

    plot_->addGraph(plot_->xAxis, plot_->yAxis2);
    plot_->graph(1)->setPen(QPen(Qt::gray, 1));
    plot_->graph(1)->setLineStyle(QCPGraph::lsLine);
    plot_->graph(1)->setScatterStyle(QCPScatterStyle::ssNone);

    //![plot Setting]
    // x_axis
    if (curr_type_ == kCameraTypeFile) {
        plot_->xAxis->setLabel(QStringLiteral("Frame"));
    } else {
        plot_->xAxis->setLabel(QStringLiteral("Time"));
        plot_->xAxis->setTickLabelType(QCPAxis::ltDateTime);
        plot_->xAxis->setDateTimeFormat("yyyy-MM-dd HH:mm:ss");
        plot_->xAxis->setAutoTickStep(true);
        plot_->xAxis->setTickLabelRotation(-45);
    }

    plot_->xAxis->setRange(x_axis_min_, x_axis_max_);

    // y_axis
    plot_->yAxis->setLabel(QStringLiteral("People Count"));
    plot_->yAxis->setRange(y_axis_min_, y_axis_max_);

    plot_->setInteraction(QCP::iRangeDrag, true);
    plot_->setInteraction(QCP::iRangeZoom, true);

    //delete below statement to allow zoom for both axis;
    plot_->axisRect()->setRangeZoom(Qt::Horizontal);

    plot_->axisRect()->setupFullAxesBox();

    connect(plot_->xAxis, SIGNAL(rangeChanged(QCPRange)), plot_->xAxis2, SLOT(setRange(QCPRange)));
    connect(plot_->yAxis, SIGNAL(rangeChanged(QCPRange)), plot_->yAxis2, SLOT(setRange(QCPRange)));

    plot_->replot();
}

void UPlotWidget::set_data(double key, double value)
{
    if (value > 100000.0) {
        return;
    }

    // Graph 0
    if (key > x_axis_max_) {
        if (curr_type_ == kCameraTypeFile) {
            x_axis_min_ = x_axis_max_ - 500.0;
            x_axis_max_ += 1000.0;

            plot_->xAxis->setRange(x_axis_min_, x_axis_max_);

        } else {
            x_axis_min_ = x_axis_max_ - 90;
            x_axis_max_ += 90.0;
            plot_->xAxis->setRange(x_axis_min_, x_axis_max_);

        }
    }

    if (value > y_axis_max_) {
        y_axis_max_ = value + 10.0;
        plot_->yAxis->setRange(y_axis_min_, y_axis_max_);
    }

    plot_->graph(0)->addData(key, value);

    // Graph 1
    plot_->yAxis2->setRange(0, 1);

    plot_->graph(1)->clearData();
    plot_->graph(1)->addData(key, 0);
    plot_->graph(1)->addData(key, 1);

    plot_->replot();
}

void UPlotWidget::clear_plot()
{
    plot_->clearGraphs();
    plot_->setInteraction(QCP::iRangeDrag, false);
    plot_->setInteraction(QCP::iRangeZoom, false);
    plot_->xAxis->setRange(0, 30);
    plot_->yAxis->setRange(0, 30);
    plot_->disconnect();

    plot_->replot();
}
