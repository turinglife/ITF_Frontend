#include "uclipcontroler.h"
#include <QLayout>
#include <QPushButton>

UClipControler::UClipControler(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *p_layout = new QHBoxLayout(this);

    // QPushButton
    p_clear_ = new QPushButton(tr("Clear"), this);
    p_start_analyze_ = new QPushButton(tr("Start"), this);
    p_stop_analyze_ = new QPushButton(tr("Stop"), this);
    p_show_analysis_ = new QPushButton(tr("Analysis"), this);
    p_setting_ = new QPushButton(tr("Setting"), this);

    p_clear_->setEnabled(false);
    p_start_analyze_->setEnabled(false);
    p_stop_analyze_->setEnabled(false);
    p_show_analysis_->setEnabled(false);
    p_setting_->setEnabled(false);

    p_layout->addWidget(p_clear_);
    p_layout->addWidget(p_start_analyze_);
    p_layout->addWidget(p_stop_analyze_);
    p_layout->addWidget(p_show_analysis_);
    p_layout->addWidget(p_setting_);
}

UClipControler::~UClipControler()
{

}

