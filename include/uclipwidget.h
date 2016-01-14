#ifndef UCLIPWIDGET_H
#define UCLIPWIDGET_H

#include <QLabel>
#include <common.h>
#include "uclipcontroler.h"

class UClipWidget : public QLabel
{
    Q_OBJECT
public:
    explicit UClipWidget(QWidget *parent = 0);
    ~UClipWidget();

    inline void set_curr_name(const string &name) {
        curr_name_ = name;
    }
    inline void set_curr_type(const string &type) {
        curr_type_ = type;
        if (type == kTaskTypeCounting || type==kTaskTypeCrossline) {
           p_clip_controler_->p_setting_->setVisible(true);
        } else {
           p_clip_controler_->p_setting_->setVisible(false);
        }
    }
    void set_clip_controler(bool fclear, bool fstart, bool fstop, bool fshow, bool fsetting);

signals:
    void ImageSizeChanged(int out_w, int out_h); /// Used to resize the image outside the widget

    void Command(QString, QString);
    void RemoveTaskFromBoard();
    void Setting();
    void ShowClipAnalysisBoard();

public slots:
    bool ShowImage(cv::Mat image); /// Used to set the image to be viewed

protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void leaveEvent(QEvent *event);
    void resizeEvent(QResizeEvent *);

private:
    void UpdateScene();
    void RenderImage();
    void ResizeImage();

    bool is_scene_changed_; /// Indicates when OpenGL view is to be redrawn
    bool is_initialized_; /// initial out_h_ and out_w_ at beginning

    QImage render_image_;

    cv::Mat orig_image_; /// original OpenCV image to be shown
    QColor bg_color_;  /// Background color

    int out_h_;  /// Resized Image height
    int out_w_; /// Resized Image width
    float img_ratio_; /// height/width ratio

    string curr_type_;
    string curr_name_;

    QLabel *p_clip_name_;
    UClipControler *p_clip_controler_;
};

#endif // UCLIPWIDGET_H
