#include "uclipwidget.h"
#include <QLayout>
#include <QMouseEvent>
#include <QDebug>

UClipWidget::UClipWidget(QWidget *parent) : QLabel(parent)
{
    is_scene_changed_ = false;
    is_initialized_ = false;
    bg_color_ = QColor::fromRgb(50, 50, 50);

    out_h_ = 0;
    out_w_ = 0;
    img_ratio_ = 4.0f/3.0f;

    setMouseTracking(true);
    setContentsMargins(2, 2, 2, 2);

    QVBoxLayout *p_layout = new QVBoxLayout(this);

    p_clip_name_ = new QLabel(this);
    p_clip_name_->setAlignment(Qt::AlignCenter);
    p_clip_controler_ = new UClipControler(this);

    connect(p_clip_controler_->p_start_analyze_, &QPushButton::clicked, [=](){
        emit Command(QString::fromStdString(kDaemonAD), QString::fromStdString(kCommandStart));
    });
    connect(p_clip_controler_->p_stop_analyze_, &QPushButton::clicked, [=](){
        emit Command(QString::fromStdString(kDaemonAD), QString::fromStdString(kCommandStop));
    });
    connect(p_clip_controler_->p_clear_, &QPushButton::clicked, [=](){
        emit RemoveTaskFromBoard();
    });
    connect(p_clip_controler_->p_show_analysis_, &QPushButton::clicked, [=](){
        emit ShowClipAnalysisBoard();
    });
    connect(p_clip_controler_->p_setting_, &QPushButton::clicked, [=](){
        emit Setting();
    });

    p_layout->addWidget(p_clip_name_);
    p_layout->addStretch();
    p_layout->addWidget(p_clip_controler_);

    p_clip_name_->hide();
    p_clip_controler_->hide();
}

UClipWidget::~UClipWidget()
{

}

void UClipWidget::set_clip_controler(bool fclear, bool fstart, bool fstop, bool fshow, bool fsetting)
{
    p_clip_controler_->p_clear_->setEnabled(fclear);
    p_clip_controler_->p_start_analyze_->setEnabled(fstart);
    p_clip_controler_->p_stop_analyze_->setEnabled(fstop);
    p_clip_controler_->p_show_analysis_->setEnabled(fshow);
    p_clip_controler_->p_setting_->setEnabled(fsetting);
}

bool UClipWidget::ShowImage(cv::Mat image)
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

    is_scene_changed_ = true;

    UpdateScene();

    return true;
}

void UClipWidget::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);
    if (!is_scene_changed_)
        return;

    RenderImage();

    is_scene_changed_ = false;
}

void UClipWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (curr_type_ == kTaskTypeCounting || curr_type_ == kTaskTypeSegmentation || curr_type_ == kTaskTypeStationary) {
        int name_area = this->size().height() * 0.15;
        if (event->pos().y() < name_area) {
            p_clip_name_->setText(QString::fromStdString(curr_name_));
            p_clip_name_->show();
        } else
            p_clip_name_->hide();

        int contrler_area = this->size().height() * 0.85;
        if (event->pos().y() > contrler_area)
            p_clip_controler_->show();
        else
            p_clip_controler_->hide();
    }
}

void UClipWidget::leaveEvent(QEvent *)
{
    if (curr_type_ == kTaskTypeCounting || curr_type_ == kTaskTypeSegmentation || curr_type_ == kTaskTypeStationary) {
        p_clip_controler_->hide();
        p_clip_name_->hide();
    }
}

void UClipWidget::resizeEvent(QResizeEvent*)
{
    ResizeImage();

    is_scene_changed_ = true;

    UpdateScene();
}

void UClipWidget::UpdateScene()
{
    if (is_scene_changed_ && this->isVisible())
        update();
}

void UClipWidget::RenderImage()
{
    if (!render_image_.isNull()) {
        if (!is_initialized_) {
            ResizeImage();
            is_initialized_ = true;
        }
        QImage image; // the image rendered

        int im_w = render_image_.width();
        int im_h = render_image_.height();

        // The image is to be resized to fit the widget
        if (im_w != this->size().width() && im_h != this->size().height()) {
            image = render_image_.scaled(
                        QSize(out_w_, out_h_),
                        Qt::IgnoreAspectRatio,
                        Qt::SmoothTransformation);
        } else {
            image = render_image_;
        }

       setPixmap(QPixmap::fromImage(image));
    }
}

void UClipWidget::ResizeImage()
{
    // ---> Scaled Image Sizes
    out_h_ = this->size().width() / img_ratio_;
    out_w_ = this->size().width();

    if (out_h_ > this->size().height()) {
        out_w_ = this->size().height()*img_ratio_;
        out_h_ = this->size().height();
    }

    emit ImageSizeChanged(out_w_, out_h_);
    // <--- Scaled Image Sizes
}
