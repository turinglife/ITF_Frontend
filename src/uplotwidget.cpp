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
    std::cout << "release UPlotWidget" << std::endl;
}

void UPlotWidget::set_plot(double x_axis_min, double x_axis_max, string camera_type, string task_type)
{
    x_axis_min_ = x_axis_min;
    x_axis_max_ = x_axis_max;
    y_axis_min_ = -1;
    y_axis_max_ = 30;
    curr_camera_type_ = camera_type;
    curr_task_type_ = task_type;

    //![Add Graph]
    plot_->addGraph(plot_->xAxis, plot_->yAxis2);
    plot_->graph(0)->setPen(QPen(Qt::gray, 1));
    plot_->graph(0)->setLineStyle(QCPGraph::lsLine);
    plot_->graph(0)->setScatterStyle(QCPScatterStyle::ssNone);

    plot_->addGraph();
    plot_->graph(1)->setName("Counting");
    plot_->graph(1)->setPen(QPen(QColor(255, 100, 0)));
    plot_->graph(1)->setLineStyle(QCPGraph::lsLine);
    plot_->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 8));

    if (curr_task_type_ == kTaskTypeCrossline) {
        plot_->addGraph();
        plot_->graph(2)->setName("Counting");
        plot_->graph(2)->setPen(QPen(QColor(0, 255, 100)));
        plot_->graph(2)->setLineStyle(QCPGraph::lsLine);
        plot_->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDiamond, 6));

        plot_->addGraph();
        plot_->graph(3)->setName("Counting");
        plot_->graph(3)->setPen(QPen(QColor(100, 0, 255)));
        plot_->graph(3)->setLineStyle(QCPGraph::lsLine);
        plot_->graph(3)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssStar, 6));
    }

    //![plot Setting]
    // x_axis
    if (curr_camera_type_ == kCameraTypeFile) {
        plot_->xAxis->setLabel(QStringLiteral("Frame"));
    } else {
        plot_->xAxis->setLabel(QStringLiteral("Time"));
        plot_->xAxis->setTickLabelType(QCPAxis::ltDateTime);
//        plot_->xAxis->setDateTimeFormat("yyyy-MM-dd HH:mm:ss");
        plot_->xAxis->setDateTimeFormat("HH:mm:ss");
        plot_->xAxis->setAutoTickStep(true);
        plot_->xAxis->setTickLabelRotation(-30);
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

    plot_->legend->setVisible(true);
    plot_->legend->removeAt(0);
    plot_->legend->setFont(QFont("Helvetica", 6));
    plot_->legend->setRowSpacing(-3);

    plot_->replot();
}

void UPlotWidget::set_data(double key, double val1, double val2, double val3)
{
    if (val1 > 100000.0) {
        return;
    }

    if (key > x_axis_max_) {
        if (curr_camera_type_ == kCameraTypeFile) {
            x_axis_min_ = x_axis_max_ - 500.0;
            x_axis_max_ += 1000.0;

            plot_->xAxis->setRange(x_axis_min_, x_axis_max_);

        } else {
            x_axis_min_ = x_axis_max_ - 90;
            x_axis_max_ += 90.0;
            plot_->xAxis->setRange(x_axis_min_, x_axis_max_);

        }
    }

    if (val1 > y_axis_max_) {
        y_axis_max_ = val1 + 10.0;
        plot_->yAxis->setRange(y_axis_min_, y_axis_max_);
    }

    // Graph 0
    plot_->yAxis2->setRange(0, 1);
    plot_->graph(0)->clearData();
    plot_->graph(0)->addData(key, 0);
    plot_->graph(0)->addData(key, 1);

    // Graph 1
    plot_->graph(1)->addData(key, val1);

    if (curr_task_type_ == kTaskTypeCrossline) {
        plot_->graph(2)->addData(key, val2);
        plot_->graph(3)->addData(key, val3);
    }

    plot_->replot();
}

void UPlotWidget::clear_plot()
{
    plot_->clearGraphs();
    plot_->setInteraction(QCP::iRangeDrag, false);
    plot_->setInteraction(QCP::iRangeZoom, false);
    plot_->xAxis->setRange(0, 30);
    plot_->yAxis->setRange(-1, 30);
    plot_->disconnect();

    plot_->replot();
}
