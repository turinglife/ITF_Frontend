#include "uttype.h"
#include <QLayout>

UTType::UTType(QWidget *parent) : UTBaseWidget(parent)
{
    QVBoxLayout *p_layout = new QVBoxLayout;

    QGridLayout *p_lay_main = new QGridLayout;
    // tasktype
    QLabel *p_lab_1 = new QLabel(tr("Task Type: "), this);
    p_tasktype_ = new QComboBox(this);
    p_tasktype_->addItem(tr("COUNTING"));
    p_tasktype_->addItem(tr("SEGMENTATION"));
    p_tasktype_->addItem(tr("STATIONARY"));
    p_tasktype_->addItem(tr("CROSSLINE"));
    p_tasktype_->setCurrentIndex(0);

    p_lay_main->addWidget(p_lab_1, 0, 0);
    p_lay_main->addWidget(p_tasktype_, 0, 1);
    p_lay_main->setColumnMinimumWidth(1, 250);

    p_layout->addWidget(p_guide_);
    p_layout->addLayout(p_lay_main);
    p_layout->addWidget(p_warning_);

    setLayout(p_layout);
}

UTType::~UTType()
{

}

