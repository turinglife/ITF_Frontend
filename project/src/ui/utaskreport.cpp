#include "utaskreport.h"
#include <QLayout>
#include <QLabel>
#include <QCheckBox>

UTaskReport::UTaskReport(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;

    // some introduction about this widget
    QVBoxLayout *lay_intro = new QVBoxLayout;
    QLabel *lab_intro = new QLabel(this);
    lab_intro->setText(tr("Create New Counting Task. Please Input the Task Name ."));
    lab_intro->setWordWrap(true);
    lay_intro->addWidget(lab_intro);

    QVBoxLayout *lay_report = new QVBoxLayout;
    report_switch_ = new QCheckBox(tr("report"), this);
    report_switch_->setChecked(false);
    lay_report->addWidget(report_switch_);

    layout->addLayout(lay_intro);
    layout->addLayout(lay_report);
    layout->addStretch();

    setLayout(layout);
}

UTaskReport::~UTaskReport()
{

}

bool UTaskReport::report_switch()
{
    return report_switch_->isChecked();
}
