#include "uclipalarmwidget.h"
#include <QLayout>
#include <QGroupBox>
#include <QPushButton>
#include "ulightwidget.h"

UClipAlarmWidget::UClipAlarmWidget(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *p_layout = new QHBoxLayout(this);

    p_light_widget_1_ = new ULightWidget(Qt::yellow, this);
    p_light_widget_2_ = new ULightWidget(Qt::red, this);
    p_light_widget_3_ = new ULightWidget(Qt::black, this);

    p_layout->addWidget(p_light_widget_1_);
    p_layout->addWidget(p_light_widget_2_);
    p_layout->addWidget(p_light_widget_3_);
}

UClipAlarmWidget::~UClipAlarmWidget()
{

}

void UClipAlarmWidget::set_alarm_strategy(bool on, std::string low, std::string medium, std::string high)
{
    alarm_on_ = on;

    if (!alarm_on_)
        return;

    low_ = atoi(low.c_str());
    medium_ = atoi(medium.c_str());
    high_ = atoi(high.c_str());

    p_light_widget_1_->setText(QString::number(low_));
    p_light_widget_2_->setText(QString::number(medium_));
    p_light_widget_3_->setText(QString::number(high_));
}

void UClipAlarmWidget::set_value(double value)
{
    if (!alarm_on_)
        return;

    p_light_widget_1_->setOn(false);
    p_light_widget_2_->setOn(false);
    p_light_widget_3_->setOn(false);

    if (value < low_)
        return;
    if (value >= low_ && value < medium_) {
        p_light_widget_1_->setOn(true);

    } else if (value >= medium_ && value < high_) {
        p_light_widget_2_->setOn(true);

    } else {
        p_light_widget_3_->setOn(true);
    }
}

