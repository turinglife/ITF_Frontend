#include "sketchpad.h"
#include <QPen>
#include <QPainter>
#include <QMouseEvent>
#include "utility.h"

Sketchpad::Sketchpad(QWidget *parent)
    : QLabel(parent)
    , draw_type_(ROI)
    , draw_finished_(true)
{
    setMouseTracking(true);

    Utility::SetLabelStyle(this);

    this->setScaledContents(true);
}

Sketchpad::~Sketchpad()
{

}

QVector<QPointF> Sketchpad::points()
{
    return points_;
}

void Sketchpad::set_points(QVector<QPointF> pts)
{
    points_.clear();
    points_ = pts;
}

void Sketchpad::set_draw_finished(bool flag)
{
    draw_finished_ = flag;
}

void Sketchpad::set_draw_type(DrawType type)
{
    points_.clear();
    if (!draw_finished_) {
        draw_finished_ = true;
    }
    update();
    if (draw_type_ != type)
        draw_type_ = type;
}

void Sketchpad::clear_sketch()
{
    points_.clear();
    update();
    draw_finished_ = true;
}

void Sketchpad::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);
    RenderSketch();
}

void Sketchpad::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !draw_finished_) {
        end_point_ = PixelToPercentage(event->pos(), this->size().width(), this->size().height());
    }

    // right button single press is undo last points
    if (event->button() == Qt::RightButton && !draw_finished_) {
        if  (!points_.isEmpty()) {
                points_.pop_back();
        }

        update();
    }
}

void Sketchpad::mouseMoveEvent(QMouseEvent *event)
{
    if (draw_type_ == GTPoints) {
        return;
    }

    end_point_ = PixelToPercentage(event->pos(), this->size().width(), this->size().height());
    update();
}

void Sketchpad::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !draw_finished_) {
        points_.push_back(end_point_);

        if (draw_type_ == LINE) {
            if (points().size() == 2) {
                draw_finished_ = true;
                emit DrawFinished();
            }
        }
        update();
    }
}

void Sketchpad::mouseDoubleClickEvent(QMouseEvent *event)
{
    // leftbutton double click to end draw
    if (event->button() == Qt::LeftButton && !draw_finished_) {
        draw_finished_ = true;

        if (!points_.isEmpty()) {
            switch (draw_type_) {
            case PersMap:
            case LINE:
                points_.pop_back();
                if (points_.size() % 2) {
                    points_.pop_back();
                }
                break;
            default:
                points_.pop_back();
                break;
            }
        }
//        std::cout << points_.size() << std::endl;
        update();

        emit DrawFinished();
    }
}

void Sketchpad::RenderSketch()
{
    QPainter painter(this);
    QPen pen;
    if (draw_type_ == GTPoints) {
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
            painter.drawPoint(PercentageToPixel(points_[i], this->size().width(), this->size().height()));
        }
        if (draw_type_ == GTPoints) {
            return;
        }
        if (draw_type_ == PersMap) {
            for (int i=0; i<points_.size()/2; ++i) {
                QPointF top_left = PercentageToPixel(points_[2*i], this->size().width(), this->size().height());
                QPointF bottom_right = PercentageToPixel(points_[2*i+1], this->size().width(), this->size().height());
                painter.drawRect(QRectF(top_left, bottom_right));
            }
        }

        if (draw_type_ == ROI || draw_type_ == LINE) {
            for (int i=0; i<points_.size()-1; ++i) {
                QPointF point1 = PercentageToPixel(points_[i], this->size().width(), this->size().height());
                QPointF point2 = PercentageToPixel(points_[i+1], this->size().width(), this->size().height());
                painter.drawLine(point1, point2);
            }
        }

        if (!draw_finished_) {
            pen.setStyle(Qt::DashDotDotLine);
            pen.setWidth(4);
            pen.setBrush(Qt::green);
            pen.setCapStyle(Qt::RoundCap);
            pen.setJoinStyle(Qt::RoundJoin);
            painter.setPen(pen);

            if (draw_type_ == PersMap) {
                if (points_.size()%2) {
                    QPointF top_left = PercentageToPixel(points_.last(), this->size().width(), this->size().height());
                    QPointF bottom_right = PercentageToPixel(end_point_, this->size().width(), this->size().height());
                    painter.drawRect(QRectF(top_left, bottom_right));
                }
            }

            if (draw_type_ == ROI || draw_type_ == LINE) {
                QPointF point1 = PercentageToPixel(points_.first(), this->size().width(), this->size().height());
                QPointF point2 = PercentageToPixel(points_.last(), this->size().width(), this->size().height());
                QPointF point3 = PercentageToPixel(end_point_, this->size().width(), this->size().height());
                painter.drawLine(point1, point3);
                painter.drawLine(point2, point3);
            }

        } else if (draw_type_ == ROI) {
            pen.setWidth(4);
            pen.setBrush(Qt::red);
            pen.setCapStyle(Qt::RoundCap);
            pen.setJoinStyle(Qt::RoundJoin);
            painter.setPen(pen);

            QPointF point1 = PercentageToPixel(points_.first(), this->size().width(), this->size().height());
            QPointF point2 = PercentageToPixel(points_.last(), this->size().width(), this->size().height());
            painter.drawLine(point1, point2);
        }
    }
}

QPointF Sketchpad::PixelToPercentage(QPointF point, qreal width, qreal height)
{
    return QPointF(point.x() / width, point.y() / height);
}

QPointF Sketchpad::PercentageToPixel(QPointF point, qreal width, qreal height)
{
    return QPointF(point.x() * width, point.y() * height);
}
