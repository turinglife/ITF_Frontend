#ifndef USKETCHPADWIDGET_H
#define USKETCHPADWIDGET_H

#include <QWidget>
#include "sketchpad.h"

class USketchPadWidget : public QWidget
{
    Q_OBJECT
public:
    explicit USketchPadWidget(QWidget *parent = 0);
    ~USketchPadWidget();

    QVector<QPointF> points();
    void set_points(QVector<QPointF> pts);
    void set_draw_finished(bool flag);
    void set_draw_type(DrawType type);
    void clear_sketchpad();

signals:
    void SketchPadFinished();

public slots:
    bool ShowImage(cv::Mat image); /// Used to set the image to be viewed

protected:
    void resizeEvent(QResizeEvent *evnet);

private:
    void ResizeSketchPad();

    QImage render_image_;
    cv::Mat orig_image_; /// original OpenCV image to be shown

    int out_h_;  /// Resized Image height
    int out_w_; /// Resized Image width
    float img_ratio_; /// height/width ratio
    Sketchpad *p_sketchpad_;
};

#endif // USKETCHPADWIDGET_H
