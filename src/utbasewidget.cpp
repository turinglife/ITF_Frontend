#include "utbasewidget.h"

UTBaseWidget::UTBaseWidget(QWidget *parent) : QWidget(parent)
{
    p_guide_ = new QLabel(this);
    p_guide_->setWordWrap(true);

    p_warning_ = new QLabel(this);
    p_warning_->setStyleSheet("color:rgb(255,0,0)");
}

UTBaseWidget::~UTBaseWidget()
{

}

