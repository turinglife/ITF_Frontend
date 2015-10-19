#include "utasktype.h"
#include <QLabel>
#include <QLayout>
#include <QComboBox>

UTaskType::UTaskType(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;

    // some introduction about this widget
    QVBoxLayout *lay_intro = new QVBoxLayout;
    QLabel *lab_intro = new QLabel(this);
    lab_intro->setText(tr("Please select a task type."));
    lab_intro->setWordWrap(true);
    lay_intro->addWidget(lab_intro);

    QGridLayout *lay_main = new QGridLayout;
    // task type
    QLabel *lab_tasktype = new QLabel(tr("Task Type: "), this);
    tasktype_ = new QComboBox(this);
    tasktype_->addItem(tr("COUNTING"));
    tasktype_->addItem(tr("SEGMENTATION"));
    tasktype_->addItem(tr("STATIONARY"));
    tasktype_->setCurrentIndex(0);

    lay_main->addWidget(lab_tasktype, 0, 0);
    lay_main->addWidget(tasktype_, 0, 1);
    lay_main->setColumnMinimumWidth(1, 250);

    // show message when task name wrong
    QVBoxLayout *lay_warning = new QVBoxLayout;
    warning_ = new QLabel(this);
    warning_->setStyleSheet("color:rgb(255,0,0)");
    lay_warning->addWidget(warning_);

    layout->addLayout(lay_intro);
    layout->addLayout(lay_main);
    layout->addLayout(lay_warning);

    setLayout(layout);
}

UTaskType::~UTaskType()
{

}

QString UTaskType::tasktype()
{
    return tasktype_->currentText().trimmed();
}

void UTaskType::set_warning(QString str)
{
    warning_->setText(str);
}

