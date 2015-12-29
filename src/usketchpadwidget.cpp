#include "usketchpadwidget.h"
#include <QLayout>

USketchPadWidget::USketchPadWidget(QWidget *parent) : QWidget(parent)
{
    out_w_ = 0;
    out_h_ = 0;
    img_ratio_ = 4.0f/3.0f;

    p_sketchpad_ = new Sketchpad(this);

    connect(p_sketchpad_, &Sketchpad::DrawFinished, [=](){
        emit SketchPadFinished();
    });
}

USketchPadWidget::~USketchPadWidget()
{
    std::cout << "release USketchPadWidget" << std::endl;
}

QVector<QPointF> USketchPadWidget::points()
{
    QVector<QPointF> pts = p_sketchpad_->points();
    for (int i=0; i<pts.size(); ++i) {
        pts[i].setX(pts[i].x() * qreal(orig_image_.cols));
        pts[i].setY(pts[i].y() * qreal(orig_image_.rows));
    }

    return pts;
}

void USketchPadWidget::set_points(QVector<QPointF> pts)
{
    for (int i=0; i<pts.size(); ++i) {
        pts[i].setX(pts[i].x() / qreal(orig_image_.cols));
        pts[i].setY(pts[i].y() / qreal(orig_image_.rows));
    }

    p_sketchpad_->set_points(pts);
}

void USketchPadWidget::set_draw_finished(bool flag)
{
    p_sketchpad_->set_draw_finished(flag);
}

void USketchPadWidget::set_draw_type(DrawType type)
{
    p_sketchpad_->set_draw_type(type);
}

void USketchPadWidget::clear_sketchpad()
{
    p_sketchpad_->clear_sketch();
}

bool USketchPadWidget::ShowImage(cv::Mat image)
{
    image.copyTo(orig_image_);

    img_ratio_ = (float)image.cols / (float)image.rows;

    if (orig_image_.channels() == 3)
        render_image_ = QImage((const unsigned char *)(orig_image_.data),
                               orig_image_.cols, orig_image_.rows,
                               orig_image_.step,
                               QImage::Format_RGB888)/*.rgbSwapped()*/;
    else if (orig_image_.channels() == 1)
        render_image_ = QImage((const unsigned char *)(orig_image_.data),
                               orig_image_.cols, orig_image_.rows,
                               orig_image_.step,
                               QImage::Format_Indexed8);
    else
        return false;

    ResizeSketchPad();

    p_sketchpad_->setPixmap(QPixmap::fromImage(render_image_));

    return true;
}

void USketchPadWidget::resizeEvent(QResizeEvent *evnet)
{
    QWidget::resizeEvent(evnet);

    ResizeSketchPad();
}

void USketchPadWidget::ResizeSketchPad()
{
    // ---> Scaled SketchPad Sizes
    out_h_ = this->size().width() / img_ratio_;
    out_w_ = this->size().width();

    if (out_h_ > this->size().height()) {
        out_w_ = this->size().height()*img_ratio_;
        out_h_ = this->size().height();
    }
    // <--- Scaled SketchPad Sizes
    // resize SketchPad
    p_sketchpad_->resize(out_w_, out_h_);

    // set SketchPad Geomoetry
    p_sketchpad_->setGeometry((this->size().width() - out_w_)/2, ( this->size().height()-out_h_)/2, out_w_, out_h_);
}
