#include "ulightwidget.h"
#include <QPainter>

ULightWidget::ULightWidget(const QColor &color, QWidget *parent)
    : QLabel(parent)
    , color_(color)
    , on_(false)
{
    setFixedSize(100, 100);
    setAlignment(Qt::AlignCenter);
}

ULightWidget::~ULightWidget()
{
}

void ULightWidget::setOn(bool on)
{
    if (on == on_)
        return;
    on_ = on;
    update();
}

void ULightWidget::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);

    if (!on_)
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(color_);
    painter.drawEllipse(5, 5, 90, 90);
}

