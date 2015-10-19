#ifndef SKETCHPAD_H
#define SKETCHPAD_H

#include <QLabel>
#include <common.h>

class Sketchpad : public QLabel
{
    Q_OBJECT
public:
    explicit Sketchpad(QWidget *parent = 0);
    ~Sketchpad();

    QVector<QPointF> points();
    void set_draw_finished(bool flag);
    void set_points(QVector<QPointF> mypoints);

signals:

public slots:
    void set_curr_type(DrawType type);
    void ResetSketchpad();

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    DrawType curr_type_;
    QPointF end_point_;
    QVector<QPointF> points_;
    bool draw_finished_;
};

#endif // SKETCHPAD_H
