#include "sketchpad.h"
#include <QPen>
#include <QPainter>
#include <QMouseEvent>

Sketchpad::Sketchpad(QWidget *parent)
    : QLabel(parent)
    , curr_type_(ROI)
    , draw_finished_(false)
{
    setMouseTracking(true);
    QSizePolicy clipSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    clipSizePolicy.setHorizontalStretch(0);
    clipSizePolicy.setVerticalStretch(0);
    clipSizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
    this->setSizePolicy(clipSizePolicy);
    this->setFocusPolicy(Qt::StrongFocus);
    this->setFrameShape(QFrame::Panel);
    this->setFrameShadow(QFrame::Plain);
    this->setLineWidth(1);
    this->setMidLineWidth(0);
    this->setAlignment(Qt::AlignCenter);
//    this->setVisible(false);
    this->setScaledContents(true);
}

Sketchpad::~Sketchpad()
{

}

QVector<QPointF> Sketchpad::points()
{
    return points_;
}

void Sketchpad::set_draw_finished(bool flag)
{
   draw_finished_ = flag;
}

void Sketchpad::set_points(QVector<QPointF> mypoints)
{
    points_ = mypoints;
    update();
}

void Sketchpad::set_curr_type(DrawType type)
{
    points_.clear();
    draw_finished_ = false;
    update();
    if (curr_type_ != type) {
        curr_type_ = type;
    }
}

void Sketchpad::ResetSketchpad()
{
    points_.clear();
    update();
    draw_finished_ = false;
}

void Sketchpad::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);

    QPainter painter(this);
    QPen pen;
    if (curr_type_ == GTPoints) {
        pen.setWidth(8);
    } else {
        pen.setWidth(4);
    }
    pen.setBrush(Qt::red);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen);

    if (!points_.empty()) {
        for (int i=0; i<points_.size(); ++i) {
            painter.drawPoint(points_[i]);
        }
        if (curr_type_ == GTPoints) {
            return;
        }

        if (curr_type_ == PersMap) {
            for (int i=0; i<points_.size()/2; ++i) {
                painter.drawRect(QRectF(points_[2*i], points_[2*i+1]));
            }
        }

        if (curr_type_ == ROI) {
            for (int i=0; i<points_.size()-1; ++i) {
                painter.drawLine(points_[i], points_[i+1]);
            }
        }

        if (!draw_finished_) {
            pen.setStyle(Qt::DashDotDotLine);
            pen.setWidth(4);
            pen.setBrush(Qt::green);
            pen.setCapStyle(Qt::RoundCap);
            pen.setJoinStyle(Qt::RoundJoin);
            painter.setPen(pen);

            if (curr_type_ == PersMap) {
                if (points_.size()%2) {
                    painter.drawRect(QRectF(points_.last(), end_point_));
                }
            }

            if (curr_type_ == ROI) {
                painter.drawLine(points_.last(), end_point_);
                painter.drawLine(points_.first(), end_point_);
            }

        } else if (curr_type_ == ROI) {
            pen.setWidth(4);
            pen.setBrush(Qt::red);
            pen.setCapStyle(Qt::RoundCap);
            pen.setJoinStyle(Qt::RoundJoin);
            painter.setPen(pen);

            painter.drawLine(points_.last(), points_[0]);
        }
    }
}

void Sketchpad::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        end_point_ = QPointF(event->pos());
    }
    if (event->button() == Qt::RightButton && !draw_finished_) {
        draw_finished_ = true;

        switch (curr_type_) {
        case PersMap:
            if (points_.size() %2) {
                points_.pop_back();
            }
            break;
        case ROI:
            if (points_.size() == 1) {
                points_.pop_back();
            }
            break;
        default:
            break;
        }

        update();
    }
}

void Sketchpad::mouseMoveEvent(QMouseEvent *event)
{
    if (curr_type_ == GTPoints) {
        return;
    }

    end_point_ = QPointF(event->pos());
    update();
}

void Sketchpad::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !draw_finished_) {
        points_.push_back(end_point_);
        update();
    }
}

void Sketchpad::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton && draw_finished_) {
        if  (!points().isEmpty()) {
            switch (curr_type_) {
            case PersMap:
                points_.pop_back();
                if (points_.size() % 2) {
                    points_.pop_back();
                }
                break;
            case ROI:
                points_.clear();
                break;
            default:
                points_.pop_back();
                break;
            }
        }

        update();
        draw_finished_ = false;
    }
}
