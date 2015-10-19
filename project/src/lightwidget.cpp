#include "lightwidget.h"
#include <QPainter>

LightWidget::LightWidget(const QColor &color, QWidget *parent)
    : QLabel(parent)
    , color_(color)
    , on_(false)
{
    setFixedSize(100, 100);
    setAlignment(Qt::AlignCenter);
}

LightWidget::~LightWidget()
{

}

bool LightWidget::isOn() const
{
    return on_;
}

void LightWidget::setOn(bool on)
{
    if (on == on_)
        return;
    on_ = on;
    update();
}

void LightWidget::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);

    if (!on_)
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(color_);
    painter.drawEllipse(5, 5, 90, 90);
//    painter.drawEllipse(0, 0, width(), height());
}

