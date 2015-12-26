#include "countingsetting.h"
#include <QLayout>
#include <QAction>
#include <QToolBar>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QMessageBox>
#include "usketchpadwidget.h"

CountingSetting::CountingSetting(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("Counting Setting");
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);

    QWidget *centralwidget = new QWidget(this);
    setCentralWidget(centralwidget);

//! [StackedWidget]
    QVBoxLayout *p_layout = new QVBoxLayout(centralwidget);
    p_stackedwidget_main_ = new QStackedWidget(this);
    p_layout->addWidget(p_stackedwidget_main_);

    //ROI
    p_sketchpad_roi_ = new USketchPadWidget(this);
    p_sketchpad_roi_->set_draw_type(ROI);
    p_stackedwidget_main_->addWidget(p_sketchpad_roi_);

    connect(p_sketchpad_roi_, SIGNAL(SketchPadFinished()), this, SLOT(OnSketchPadFinished()));
    //Pers
    p_sketchpad_pers_ = new USketchPadWidget(this);
    p_sketchpad_pers_->set_draw_type(PersMap);
    p_stackedwidget_main_->addWidget(p_sketchpad_pers_);

    connect(p_sketchpad_pers_, SIGNAL(SketchPadFinished()), this, SLOT(OnSketchPadFinished()));
    //GT
    p_stackedwidget_gt_ = new QStackedWidget(this);
    p_stackedwidget_main_->addWidget(p_stackedwidget_gt_);

    p_stackedwidget_main_->setCurrentIndex(0);

//! [Action] [TOOLBAR]
    p_draw_ = new QAction(tr("Draw ROI"), this);
    p_snapshot_ = new QAction(tr("Snapshot"), this);
    p_clear_ = new QAction(tr("Clear All"), this);
    p_delete_ = new QAction(tr("Delete"), this);
    p_delete_->setVisible(false);
    connect(p_draw_, SIGNAL(triggered()), this, SLOT(OnActionDrawTriggered()));
    connect(p_snapshot_, SIGNAL(triggered()), this, SLOT(OnActionSnapshotTriggered()));
    connect(p_clear_, SIGNAL(triggered()), this, SLOT(OnActionClearTriggered()));
    connect(p_delete_, SIGNAL(triggered()), this, SLOT(OnActionDeleteTriggered()));
    p_toolbar_ = addToolBar("Action");
    p_toolbar_->addAction(p_draw_);
    p_toolbar_->addAction(p_snapshot_);
    p_toolbar_->addAction(p_clear_);
    p_toolbar_->addAction(p_delete_);

//! [QPushButton]
    QHBoxLayout *p_lay_btn = new QHBoxLayout;
    p_back_ = new QPushButton(tr("<<Back"), this);
    p_back_->setHidden(true);
    p_next_ = new QPushButton(tr("Next>>"), this);
    p_gt_prev_ = new QPushButton(tr("<"), this);
    p_gt_prev_->setHidden(true);
    p_gt_num_ = new QLabel(this);
    p_gt_num_->setHidden(true);
    p_gt_next_ = new QPushButton(tr(">"),this);
    p_gt_next_->setHidden(true);

    connect(p_back_, SIGNAL(clicked()), this, SLOT(OnBtnBackClicked()));
    connect(p_next_, SIGNAL(clicked()), this, SLOT(OnBtnNextClicked()));
    connect(p_gt_prev_, SIGNAL(clicked()), this, SLOT(OnBtnGtPrevClicked()));
    connect(p_gt_next_, SIGNAL(clicked()), this, SLOT(OnBtnGtNextClicked()));

    p_lay_btn->addWidget(p_back_);
    p_lay_btn->addStretch();
    p_lay_btn->addWidget(p_gt_prev_);
    p_lay_btn->addWidget(p_gt_num_);
    p_lay_btn->addWidget(p_gt_next_);
    p_lay_btn->addStretch();
    p_lay_btn->addWidget(p_next_);

    p_layout->addLayout(p_lay_btn);
}

CountingSetting::~CountingSetting()
{

}

QVector<QPointF> CountingSetting::roi_points() const
{
    return p_sketchpad_roi_->points();
}

QVector<QPointF> CountingSetting::pers_points() const
{
    return p_sketchpad_pers_->points();
}

QVector<QVector<QPointF> > CountingSetting::gt_points() const
{
    QVector<QVector<QPointF> > pts;
    for (int i=0; i<sketchpad_gt_.size(); ++i) {
        pts.append(sketchpad_gt_[i]->points());
    }

    return pts;
}

QVector<cv::Mat> CountingSetting::gt_images() const
{
    return img_gt_;
}

void CountingSetting::set_roi_points_images(cv::Mat img, QVector<QPointF> pts)
{
    p_sketchpad_roi_->set_draw_finished(true);

    p_sketchpad_roi_->ShowImage(img); /// ShowImage need to process before set_points, since set_points need images' rows and cols;

    p_sketchpad_roi_->set_points(pts);
}

void CountingSetting::set_pers_points_images(cv::Mat img, QVector<QPointF> pts)
{
    p_sketchpad_pers_->set_draw_finished(true);

    p_sketchpad_pers_->ShowImage(img); /// ShowImage need to process before set_points, since set_points need images' rows and cols;

    p_sketchpad_pers_->set_points(pts);
}

void CountingSetting::set_gt_points_images(QVector<cv::Mat> imgs, QVector<QVector<QPointF> > pts)
{
    img_gt_ = imgs;

    for(int i=0; i<img_gt_.size(); ++i) {
        cv::Mat img = img_gt_[i];
        USketchPadWidget *p_sketchpad = new USketchPadWidget(this);
        p_sketchpad->set_draw_type(GTPoints);
        p_sketchpad->ShowImage(img); /// ShowImage need to process before set_points, since set_points need images' rows and cols;
        p_sketchpad->set_points(pts[i]);
        sketchpad_gt_.push_back(p_sketchpad);

        p_stackedwidget_gt_->addWidget(sketchpad_gt_.last());
        p_stackedwidget_gt_->setCurrentIndex(p_stackedwidget_gt_->count() - 1);

        connect(sketchpad_gt_.last(), SIGNAL(SketchPadFinished()), this, SLOT(OnSketchPadFinished()));
    }

    UpdateGTNum();
}

void CountingSetting::Snapshot(cv::Mat img)
{
    if (p_stackedwidget_main_->currentIndex() == 0) {
        p_sketchpad_roi_->clear_sketchpad();
        p_sketchpad_roi_->ShowImage(img);
    } else if (p_stackedwidget_main_->currentIndex() == 1) {
        p_sketchpad_pers_->clear_sketchpad();
        p_sketchpad_pers_->ShowImage(img);
    } else if (p_stackedwidget_main_->currentIndex() == 2) {
        img_gt_.push_back(img.clone());
        USketchPadWidget *p_sketchpad = new USketchPadWidget(this);
        p_sketchpad->set_draw_type(GTPoints);
        p_sketchpad->ShowImage(img_gt_.last());
        sketchpad_gt_.push_back(p_sketchpad);

        p_stackedwidget_gt_->addWidget(sketchpad_gt_.last());
        p_stackedwidget_gt_->setCurrentIndex(p_stackedwidget_gt_->count() - 1);
        connect(sketchpad_gt_.last(), SIGNAL(SketchPadFinished()), this, SLOT(OnSketchPadFinished()));
    } else {
        return;
    }

    UpdateGTNum();
}

void CountingSetting::OnActionDrawTriggered()
{
    // when draw in processing, all buttom, action need to disable
    UpdateBtnStatus(false);

    if (p_stackedwidget_main_->currentIndex() == 0) {
        p_sketchpad_roi_->set_draw_finished(false);
    } else if (p_stackedwidget_main_->currentIndex() == 1) {
        p_sketchpad_pers_->set_draw_finished(false);
    } else if (p_stackedwidget_main_->currentIndex() == 2) {
        if (img_gt_.isEmpty()) {
            emit ActionSnapshotTriggered();
            return;
        }
        sketchpad_gt_[p_stackedwidget_gt_->currentIndex()]->set_draw_finished(false);
    } else {
       return;
    }
}

void CountingSetting::OnActionSnapshotTriggered()
{
    emit ActionSnapshotTriggered();
}

void CountingSetting::OnActionClearTriggered()
{
    if (p_stackedwidget_main_->currentIndex() == 0) {
        p_sketchpad_roi_->clear_sketchpad();
    } else if (p_stackedwidget_main_->currentIndex() == 1) {
        p_sketchpad_pers_->clear_sketchpad();
    } else if (p_stackedwidget_main_->currentIndex() == 2) {
        if (p_stackedwidget_gt_->count() == 0) {
            return;
        }
        sketchpad_gt_[p_stackedwidget_gt_->currentIndex()]->clear_sketchpad();
    } else {
        return;
    }
}

void CountingSetting::OnActionDeleteTriggered()
{
    if (p_stackedwidget_main_->currentIndex() != 2)
        return;
    if (p_stackedwidget_gt_->count() == 0) {
        return;
    }
    // mark the index of current gt sketchpad
    int index = p_stackedwidget_gt_->currentIndex();

    //remove current gt sketchpad from p_stackedwidget_gt_
    p_stackedwidget_gt_->removeWidget(p_stackedwidget_gt_->currentWidget());

    // remove this sketchpad from sketchpad_gt and delete it
    USketchPadWidget *p_sketchpad = sketchpad_gt_.at(index);
    sketchpad_gt_.remove(index);
    delete p_sketchpad;

    //remove image of this sketchpad
    img_gt_.remove(index);

    UpdateGTNum();

    if (p_stackedwidget_gt_->count() == 0) {
        emit ActionSnapshotTriggered();
    }
}

void CountingSetting::OnBtnBackClicked()
{
    if (p_stackedwidget_main_->currentIndex() == 1) {
        p_stackedwidget_main_->setCurrentIndex(0);
        p_draw_->setText(tr("Draw ROI"));
        p_back_->setHidden(true);
    } else if (p_stackedwidget_main_->currentIndex() == 2) {
        p_draw_->setText(tr("Draw Pers"));
        p_stackedwidget_main_->setCurrentIndex(1);
        p_next_->setText(tr("Next>>"));
        p_gt_prev_->setHidden(true);
        p_gt_num_->setHidden(true);
        p_gt_next_->setHidden(true);
        p_delete_->setVisible(false);
    } else {
        return;
    }
}

void CountingSetting::OnBtnNextClicked()
{
    if (p_stackedwidget_main_->currentIndex() == 0) {
        if (p_sketchpad_roi_->points().isEmpty()) {
            QMessageBox::information(NULL, "Counting Setting", "Please Draw ROI!", QMessageBox::Ok, QMessageBox::Ok);
            return;
        }
        p_stackedwidget_main_->setCurrentIndex(1);
        p_draw_->setText(tr("Draw Pers"));
        p_back_->setHidden(false);

        // if p_sketchpad_pers_'s points is empty, then need snapshot
        if (p_sketchpad_pers_->points().empty()) {
            emit ActionSnapshotTriggered();
        }
    } else if (p_stackedwidget_main_->currentIndex() == 1) {
        if (p_sketchpad_pers_->points().size()/2 < 2) {
            QMessageBox::information(NULL, "Counting Setting", "Please Draw Pers!", QMessageBox::Ok, QMessageBox::Ok);
            return;
        }
        p_stackedwidget_main_->setCurrentIndex(2);
        p_draw_->setText(tr("Label GT"));
        p_next_->setText(tr("Finish"));
        p_gt_prev_->setHidden(false);
        p_gt_num_->setHidden(false);
        p_gt_next_->setHidden(false);
        p_delete_->setVisible(true);

        // if gt is empty, then need snapshot
        if (img_gt_.empty()) {
            emit ActionSnapshotTriggered();
        }
    } else if (p_stackedwidget_main_->currentIndex() == 2) {
        // counting setting finished, emit signal to delegate
        if (sketchpad_gt_.size() < 2) {
            QMessageBox::information(NULL, "Counting Setting", "Please Label at least two Images!", QMessageBox::Ok, QMessageBox::Ok);
            return;
        }
        emit CountingSettingFinished();
    } else {
        return;
    }
}

void CountingSetting::OnBtnGtPrevClicked()
{
    if (p_stackedwidget_gt_->count() == 0) {
        return;
    }

    p_stackedwidget_gt_->setCurrentIndex(p_stackedwidget_gt_->currentIndex() - 1);

    UpdateGTNum();
}

void CountingSetting::OnBtnGtNextClicked()
{
    if (p_stackedwidget_gt_->count() == 0) {
        return;
    }

    p_stackedwidget_gt_->setCurrentIndex(p_stackedwidget_gt_->currentIndex() + 1);

    UpdateGTNum();
}

void CountingSetting::OnSketchPadFinished()
{
    UpdateBtnStatus(true);
}

void CountingSetting::UpdateBtnStatus(bool flag)
{
    p_snapshot_->setEnabled(flag);
    p_draw_->setEnabled(flag);
    p_clear_->setEnabled(flag);
    p_delete_->setEnabled(flag);
    p_back_->setEnabled(flag);
    p_next_->setEnabled(flag);
    p_gt_prev_->setEnabled(flag);
    p_gt_next_->setEnabled(flag);
}

void CountingSetting::UpdateGTNum()
{
    p_gt_num_->setText(QString("%1 / %2").arg(p_stackedwidget_gt_->currentIndex() + 1).arg(p_stackedwidget_gt_->count()));
}

