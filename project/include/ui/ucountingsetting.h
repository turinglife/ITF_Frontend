#ifndef UCOUNTINGSETTING_H
#define UCOUNTINGSETTING_H

#include <QWidget>
#include "common.h"

class QStackedWidget;
class QPushButton;
class Sketchpad;
class QLabel;

class UCountingSetting : public QWidget
{
    Q_OBJECT
public:
    explicit UCountingSetting(QWidget *parent = 0);
    ~UCountingSetting();

    QVector<QPointF> roi_points();
    QVector<QPointF> pers_points();
    QVector<QVector<QPointF> > gt_points();
    QVector<cv::Mat> gt_images();

    void set_roi_points(cv::Mat img, QVector<QPointF> mypoints);
    void set_pers_points(cv::Mat img, QVector<QPointF> mypoints);
    void set_gt_points(QVector<cv::Mat> imgs, QVector<QVector<QPointF> > mypoints);

signals:
    void BtnSnapshotClicked();
    void BtnFinishClicked();

public slots:
    void Snapshot(cv::Mat);

private slots:
    void OnBtnSnapshotClicked();
    void OnBtnDeleteClicked();
    void OnBtnNextClicked();
    void OnBtnBackClicked();
    void OnBtnGTNextClicked();
    void OnBtnGTPrevClicked();

private:
    void RemoveGTSketchpad(int index);
    void UpdateGTSketchpadNum();
    void ReadCountingSetting();
    void WriteCountingSetting();

    QStackedWidget *stackedwidget_;
    Sketchpad *sketchpad_pers_;
    Sketchpad *sketchpad_roi_;

    QStackedWidget *stackedwidget_gt_;
    QVector<cv::Mat> img_gt_;
    QVector<Sketchpad* > sketchpad_gt_;

    QPushButton *btn_back_;
    QPushButton *btn_next_;
    QPushButton *btn_gt_prev_;
    QPushButton *btn_gt_next_;
    QLabel *lab_gt_num_;

    int img_height;
    int img_width;

};

#endif // UCOUNTINGSETTING_H
