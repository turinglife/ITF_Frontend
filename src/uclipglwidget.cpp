#include "uclipglwidget.h"
#include <QOpenGLFunctions>
#include <QLayout>
#include <QMouseEvent>

UClipGLWidget::UClipGLWidget(QWidget *parent) : QGLWidget(parent)
{
    is_scene_changed_ = false;
    bg_color_ = QColor::fromRgb(150, 150, 150);

    out_h_ = 0;
    out_w_ = 0;
    img_ratio_ = 4.0f/3.0f;

    pos_x_ = 0;
    pos_y_ = 0;

    setMouseTracking(true);

    QVBoxLayout *p_layout = new QVBoxLayout(this);

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

    p_layout->addStretch();
    p_layout->addWidget(p_clip_controler_);

    p_clip_controler_->hide();
}

UClipGLWidget::~UClipGLWidget()
{
}

void UClipGLWidget::set_clip_controler(bool fclear, bool fstart, bool fstop, bool fshow, bool fsetting)
{
    p_clip_controler_->p_clear_->setEnabled(fclear);
    p_clip_controler_->p_start_analyze_->setEnabled(fstart);
    p_clip_controler_->p_stop_analyze_->setEnabled(fstop);
    p_clip_controler_->p_show_analysis_->setEnabled(fshow);
    p_clip_controler_->p_setting_->setEnabled(fsetting);
}

bool UClipGLWidget::ShowImage(cv::Mat image)
{
    cv::flip(image, image, 0);

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

void UClipGLWidget::initializeGL()
{
    makeCurrent();

    QOpenGLFunctions *p_func = QOpenGLContext::currentContext()->functions();
    float r = ((float)bg_color_.darker().red())/255.0f;
    float g = ((float)bg_color_.darker().green())/255.0f;
    float b = ((float)bg_color_.darker().blue())/255.0f;
    p_func->glClearColor(r,g,b,1.0f);
}

void UClipGLWidget::paintGL()
{
    makeCurrent();

    if (!is_scene_changed_)
        return;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderImage();

    is_scene_changed_ = false;
}

void UClipGLWidget::resizeGL(int width, int height)
{
    makeCurrent();
    glViewport(0, 0, (GLint)width, (GLint)height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // To Draw Image in the center of the area
    glOrtho(0, width, 0, height, 0, 1);

    glMatrixMode(GL_MODELVIEW);

    // ---> Scaled Image Sizes
    out_h_ = width / img_ratio_;
    out_w_ = width;

    if (out_h_ > height) {
        out_w_ = height*img_ratio_;
        out_h_ = height;
    }

    emit ImageSizeChanged(out_w_, out_h_);
    // <--- Scaled Image Sizes

    pos_x_ = (width - out_w_)/2;
    pos_y_ = (height - out_h_)/2;

    is_scene_changed_ = true;

    UpdateScene();
}

void UClipGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (curr_type_ == kTaskTypeCounting || curr_type_ == kTaskTypeSegmentation || curr_type_ == kTaskTypeStationary) {
        int active_area = this->size().height() * 0.85;
        if (event->pos().y() > active_area)
            p_clip_controler_->show();
        else
            p_clip_controler_->hide();
    }
}

void UClipGLWidget::leaveEvent(QEvent *)
{
    if (curr_type_ == kTaskTypeCounting || curr_type_ == kTaskTypeSegmentation || curr_type_ == kTaskTypeStationary) {
        p_clip_controler_->hide();
    }
}

void UClipGLWidget::UpdateScene()
{
    if (is_scene_changed_ && this->isVisible())
        update();
}

void UClipGLWidget::RenderImage()
{
    makeCurrent();

    glClear(GL_COLOR_BUFFER_BIT);

    if (!render_image_.isNull()) {
        glLoadIdentity();

        QImage image; // the image rendered

        glPushMatrix();
        {
            int im_w = render_image_.width();
            int im_h = render_image_.height();

            // The image is to be resized to fit the widget
            if (im_w != this->size().width() && im_h != this->size().height()) {
                image = render_image_.scaled(
                            QSize(out_w_, out_h_),
                            Qt::IgnoreAspectRatio,
                            Qt::SmoothTransformation);
            }
            else
                image = render_image_;

            // ---> Centering image in draw area
            glRasterPos2i(pos_x_, pos_y_);
            // <--- Centering image in draw area

            im_w = image.width();
            im_h = image.height();

            glDrawPixels(im_w, im_h, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
        }
        glPopMatrix();

        // end
        glFlush();
    }
}

