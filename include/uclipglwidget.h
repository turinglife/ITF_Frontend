#ifndef UCLIPGLWIDGET_H
#define UCLIPGLWIDGET_H

#include "common.h"
#include <QtOpenGL/QGLWidget>
#include "uclipcontroler.h"

class UClipGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit UClipGLWidget(QWidget *parent = 0);
    ~UClipGLWidget();

    inline void set_curr_name(const string &name) {
        curr_name_ = name;
    }
    inline void set_curr_type(const string &type) {
        curr_type_ = type;
    }
    void set_clip_controler(bool fclear, bool fstart, bool fstop, bool fshow, bool fsetting);

signals:
    void ImageSizeChanged( int out_w, int out_h ); /// Used to resize the image outside the widget

    void Command(QString, QString);
    void RemoveTaskFromBoard();
    void Setting();
    void ShowClipAnalysisBoard();

public slots:
    bool ShowImage(cv::Mat image); /// Used to set the image to be viewed

protected:
    void initializeGL(); /// OpenGL initialization
    void paintGL();  /// OpenGL Rendering
    void resizeGL(int width, int height); /// Widget Resize Event

    void mouseMoveEvent(QMouseEvent *event);
    void leaveEvent(QEvent *);

private:
    void UpdateScene();
    void RenderImage();

    bool is_scene_changed_; /// Indicates when OpenGL view is to be redrawn

    QImage render_image_;          /// Qt image to be rendered
    cv::Mat orig_image_; /// original OpenCV image to be shown
    QColor bg_color_;  /// Background color

    int out_h_;  /// Resized Image height
    int out_w_; /// Resized Image width
    float img_ratio_; /// height/width ratio

    int pos_x_; /// Top left X position to render image in the center of widget
    int pos_y_; /// Top left Y position to render image in the center of widget

    string curr_type_;
    string curr_name_;

    UClipControler *p_clip_controler_;
};

#endif // UCLIPGLWIDGET_H
