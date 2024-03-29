#ifndef COUNTINGSETTING_H
#define COUNTINGSETTING_H

#include <QMainWindow>
#include "common.h"

class QPushButton;
class QLabel;
class QStackedWidget;
class USketchPadWidget;
class QToolBar;

class CountingSetting : public QMainWindow
{
    Q_OBJECT
public:
    explicit CountingSetting(std::string task_type, QWidget *parent = 0);
    ~CountingSetting();

    QVector<QPointF> roi_points() const;
    QVector<QPointF> pers_points() const;
    QVector<QVector<QPointF> > gt_points() const;
    QVector<cv::Mat> gt_images() const;

    void set_roi_points_images(cv::Mat img, QVector<QPointF> pts);
    void set_pers_points_images(cv::Mat img, QVector<QPointF> pts);
    void set_gt_points_images(QVector<cv::Mat> imgs, QVector<QVector<QPointF> > pts);

signals:
    void ActionSnapshotTriggered();
    void CountingSettingFinished();

public slots:
    void Snapshot(cv::Mat img);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void OnActionDrawTriggered();
    void OnActionSnapshotTriggered();
    void OnActionClearTriggered();
    void OnActionDeleteTriggered();
    void OnBtnBackClicked();
    void OnBtnNextClicked();
    void OnBtnGtPrevClicked();
    void OnBtnGtNextClicked();
    void OnSketchPadFinished();

private:
    void ImageWithROI(cv::Mat &img);
    void UpdateBtnStatus(bool flag);
    void UpdateGTNum();
    void release_memory();
    void set_btn_visible(bool flag_draw, bool flag_snapshot, bool flag_clear, bool flag_delete, bool flag_back, bool flag_next, bool flag_gt_prev, bool flag_gt_next, bool flag_gt_num);

    QToolBar *p_toolbar_;

    QStackedWidget *p_stackedwidget_main_;
    QStackedWidget *p_stackedwidget_gt_;

    USketchPadWidget *p_sketchpad_pers_;
    USketchPadWidget *p_sketchpad_roi_;
    QVector<USketchPadWidget *> sketchpad_gt_;
    QVector<cv::Mat> img_gt_;

    QAction *p_draw_;
    QAction *p_snapshot_;
    QAction *p_clear_;
    QAction *p_delete_;

    QPushButton *p_back_;
    QPushButton *p_next_;
    QPushButton *p_gt_prev_;
    QPushButton *p_gt_next_;
    QLabel *p_gt_num_;

    std::string task_type_;
};

#endif // COUNTINGSETTING_H
