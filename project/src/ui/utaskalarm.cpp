#include "utaskalarm.h"
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QValidator>

UTaskAlarm::UTaskAlarm(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;

    // some introduction about this widget
    QVBoxLayout *lay_intro = new QVBoxLayout;
    QLabel *lab_intro = new QLabel(this);
    lab_intro->setText(tr("Create New Counting Task. Please Input the Task Name ."));
    lab_intro->setWordWrap(true);
    lay_intro->addWidget(lab_intro);

    QGridLayout *lay_alarm = new QGridLayout;
    alarm_switch_ = new QCheckBox(tr("alarm switch"), this);
    alarm_switch_->setChecked(true);

    QIntValidator *validator = new QIntValidator(this);
    validator->setBottom(0);

    QLabel *lab_low = new QLabel(tr("low"),this);
    priority_low_ = new QLineEdit(this);
    priority_low_->setValidator(validator);
    QLabel *lab_medium = new QLabel(tr("medium"), this);
    priority_medium_ = new QLineEdit(this);
    priority_medium_->setValidator(validator);
    QLabel *lab_high = new QLabel(tr("high"), this);
    priority_high_ = new QLineEdit(this);
    priority_high_->setValidator(validator);

    lay_alarm->addWidget(alarm_switch_, 0, 0);
    lay_alarm->addWidget(lab_low, 1, 0);
    lay_alarm->addWidget(priority_low_, 1, 1);
    lay_alarm->addWidget(lab_medium, 2, 0);
    lay_alarm->addWidget(priority_medium_, 2, 1);
    lay_alarm->addWidget(lab_high, 3, 0);
    lay_alarm->addWidget(priority_high_, 3, 1);

    QVBoxLayout *lay_warning = new QVBoxLayout;
    warning_ = new QLabel(this);
    warning_->setStyleSheet("color:rgb(255,0,0)");
    lay_warning->addWidget(warning_);

    layout->addLayout(lay_intro);
    layout->addLayout(lay_alarm);
    layout->addLayout(lay_warning);

    setLayout(layout);

    connect(alarm_switch_, SIGNAL(clicked()), this, SLOT(on_CheckBox_clicked()));
}

UTaskAlarm::~UTaskAlarm()
{

}

bool UTaskAlarm::alarm_switch()
{
    return alarm_switch_->isChecked();
}

int UTaskAlarm::priority_low()
{
    if (priority_low_->text().isEmpty()) {
        return -1;
    } else {
        return priority_low_->text().toInt();
    }
}

int UTaskAlarm::priority_medium()
{
    if (priority_medium_->text().isEmpty()) {
        return -1;
    } else {
        return priority_medium_->text().toInt();
    }
}

int UTaskAlarm::priority_high()
{
    if (priority_high_->text().isEmpty()) {
        return -1;
    } else {
        return priority_high_->text().toInt();
    }
}

void UTaskAlarm::set_warning(QString str)
{
    warning_->setText(str);
}

void UTaskAlarm::on_CheckBox_clicked()
{
    warning_->clear();

    if (alarm_switch_->isChecked()) {
        priority_low_->setEnabled(true);
        priority_medium_->setEnabled(true);
        priority_high_->setEnabled(true);
    } else {
        priority_low_->setEnabled(false);
        priority_medium_->setEnabled(false);
        priority_high_->setEnabled(false);
        priority_low_->clear();
        priority_medium_->clear();
        priority_high_->clear();
    }
}


