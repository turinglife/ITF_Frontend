#include "utalarmcounting.h"
#include <QLayout>
#include <QValidator>

UTAlarmCounting::UTAlarmCounting(QWidget *parent) : UTAlarmStrategy(parent)
{
    QVBoxLayout *p_layout = new QVBoxLayout;

    QGridLayout *p_lay_main = new QGridLayout;
    // counting alarm
    p_alarm_switch_ = new QCheckBox(tr("Alarm Switch"), this);
    p_alarm_switch_->setChecked(false);

    QIntValidator *validator = new QIntValidator(this);
    validator->setBottom(0);

    QLabel *p_lab_1 = new QLabel(tr("Low"),this);
    p_priority_low_ = new QLineEdit(this);
    p_priority_low_->setValidator(validator);
    QLabel *p_lab_2 = new QLabel(tr("Medium"), this);
    p_priority_medium_ = new QLineEdit(this);
    p_priority_medium_->setValidator(validator);
    QLabel *p_lab_3 = new QLabel(tr("High"), this);
    p_priority_high_ = new QLineEdit(this);
    p_priority_high_->setValidator(validator);

    p_priority_low_->setEnabled(false);
    p_priority_medium_->setEnabled(false);
    p_priority_high_->setEnabled(false);

    p_lay_main->addWidget(p_alarm_switch_, 0, 0);
    p_lay_main->addWidget(p_lab_1, 1, 0);
    p_lay_main->addWidget(p_priority_low_, 1, 1);
    p_lay_main->addWidget(p_lab_2, 2, 0);
    p_lay_main->addWidget(p_priority_medium_, 2, 1);
    p_lay_main->addWidget(p_lab_3, 3, 0);
    p_lay_main->addWidget(p_priority_high_, 3, 1);
    p_lay_main->setColumnMinimumWidth(1, 250);

    p_layout->addWidget(p_guide_);
    p_layout->addLayout(p_lay_main);
    p_layout->addWidget(p_warning_);

    setLayout(p_layout);

    //set connection
    connect(p_alarm_switch_, SIGNAL(clicked()), this, SLOT(OnCheckBoxClicked()));
}

UTAlarmCounting::~UTAlarmCounting()
{

}

void UTAlarmCounting::OnCheckBoxClicked()
{
    p_warning_->clear();

    if (p_alarm_switch_->isChecked()) {
        p_priority_low_->setEnabled(true);
        p_priority_medium_->setEnabled(true);
        p_priority_high_->setEnabled(true);
    } else {
        p_priority_low_->setEnabled(false);
        p_priority_medium_->setEnabled(false);
        p_priority_high_->setEnabled(false);
        p_priority_low_->clear();
        p_priority_medium_->clear();
        p_priority_high_->clear();
    }
}

