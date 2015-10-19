#include "alarmwidget.h"
#include <QLayout>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include "lightwidget.h"

AlarmWidget::AlarmWidget(QWidget *parent) : QWidget(parent)
{
//! [QSizePolicy]
    QVBoxLayout *layout = new QVBoxLayout;

    QHBoxLayout *lay_alarm = new QHBoxLayout;
    lay_alarm->setSpacing(0);
    lay_alarm->setContentsMargins(0, 0, 0, 0);
    QLabel *lab_alarm = new QLabel(tr("Alarm"), this);
    lay_alarm->addWidget(lab_alarm);

    QGroupBox *gbox_sign = new QGroupBox(this);
//    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    sizePolicy1.setHorizontalStretch(0);
//    sizePolicy1.setVerticalStretch(0);
//    sizePolicy1.setHeightForWidth(gbox_sign->sizePolicy().hasHeightForWidth());
//    gbox_sign->setSizePolicy(sizePolicy1);
    gbox_sign->setMinimumSize(QSize(250, 0));
    QHBoxLayout *lay_sign = new QHBoxLayout(gbox_sign);
    lay_sign->setSpacing(0);
    lay_sign->setContentsMargins(0, 0, 0, 0);

    green_ = new LightWidget(Qt::yellow, this);
    yellow_ = new LightWidget(Qt::red, this);
    red_ = new LightWidget(Qt::black, this);

    lay_sign->addWidget(green_);
    lay_sign->addWidget(yellow_);
    lay_sign->addWidget(red_);

    QPushButton *btn_alarm = new QPushButton(tr("setting"), this);
    btn_alarm->setVisible(false);

    layout->addLayout(lay_alarm);
    layout->addWidget(gbox_sign);
    layout->addWidget(btn_alarm);

    setLayout(layout);

    connect(btn_alarm, SIGNAL(clicked()), this, SLOT(OnAlarmBtnClicked()));
}

AlarmWidget::~AlarmWidget()
{

}

void AlarmWidget::set_alarm_strategy(bool on, std::string low, std::string medium, std::string high)
{
    alarm_on_ = on;

    if (!alarm_on_)
        return;

    low_ = atoi(low.c_str());
    medium_ = atoi(medium.c_str());
    high_ = atoi(high.c_str());

    green_->setText(QString::number(low_));
    yellow_->setText(QString::number(medium_));
    red_->setText(QString::number(high_));
}

void AlarmWidget::set_value(double value)
{
    if (!alarm_on_)
        return;

    green_->setOn(false);
    yellow_->setOn(false);
    red_->setOn(false);

    if (value < low_)
        return;
    if (value >= low_ && value < medium_) {
        green_->setOn(true);

    } else if (value >= medium_ && value < high_) {
        yellow_->setOn(true);

    } else {
        red_->setOn(true);

    }
}

void AlarmWidget::OnAlarmBtnClicked()
{
}
