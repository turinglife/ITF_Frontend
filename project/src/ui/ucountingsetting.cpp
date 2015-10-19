#include "ucountingsetting.h"
#include <QStackedWidget>
#include <QLayout>
#include <QPushButton>
#include "sketchpad.h"
#include <QDebug>
#include <QMessageBox>


UCountingSetting::UCountingSetting(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;

    //ROI
    QWidget *widget_roi = new QWidget(this);
    QHBoxLayout *lay_roi = new QHBoxLayout(widget_roi);
    sketchpad_roi_ = new Sketchpad(this);
    sketchpad_roi_->set_curr_type(ROI);
    sketchpad_roi_->setFixedSize(kSketchpadWidth, kSketchpadHeight);

    lay_roi->addWidget(sketchpad_roi_);

    // Persmap
    QWidget *widget_pers = new QWidget(this);
    QHBoxLayout *lay_pers = new QHBoxLayout(widget_pers);
    sketchpad_pers_ = new Sketchpad(this);
    sketchpad_pers_->set_curr_type(PersMap);
    sketchpad_pers_->setFixedSize(kSketchpadWidth, kSketchpadHeight);

    lay_pers->addWidget(sketchpad_pers_);

    //GT
    QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);

    QWidget *widget_gt = new QWidget(this);
    QVBoxLayout *lay_gt = new QVBoxLayout(widget_gt);
    QHBoxLayout *lay_gt_main = new QHBoxLayout;
    stackedwidget_gt_ = new QStackedWidget(this);

    lay_gt_main->addWidget(stackedwidget_gt_);

    lay_gt->addLayout(lay_gt_main);


    btn_gt_prev_ = new QPushButton(tr("<"), this);
    sizePolicy1.setHeightForWidth(btn_gt_prev_->sizePolicy().hasHeightForWidth());
    btn_gt_prev_->setSizePolicy(sizePolicy1);
    btn_gt_prev_->setHidden(true);

    btn_gt_next_ = new QPushButton(tr(">"), this);
    sizePolicy1.setHeightForWidth(btn_gt_next_->sizePolicy().hasHeightForWidth());
    btn_gt_next_->setSizePolicy(sizePolicy1);
    btn_gt_next_->setHidden(true);

    lab_gt_num_ = new QLabel(this);
    sizePolicy1.setHeightForWidth(lab_gt_num_->sizePolicy().hasHeightForWidth());
    lab_gt_num_->setSizePolicy(sizePolicy1);
    lab_gt_num_->setHidden(true);
    UpdateGTSketchpadNum();


    // For ROI, Pers and GT
    QHBoxLayout *lay_btn = new QHBoxLayout;
    QPushButton *btn_snapshot = new QPushButton(tr("snapshot"), this);
    sizePolicy1.setHeightForWidth(btn_snapshot->sizePolicy().hasHeightForWidth());
    btn_snapshot->setSizePolicy(sizePolicy1);

    QPushButton *btn_delete = new QPushButton(tr("delete"), this);
    sizePolicy1.setHeightForWidth(btn_delete->sizePolicy().hasHeightForWidth());
    btn_delete->setSizePolicy(sizePolicy1);

    btn_back_ = new QPushButton(tr("<<Back"), this);
    btn_back_->setEnabled(false);
    sizePolicy1.setHeightForWidth(btn_back_->sizePolicy().hasHeightForWidth());
    btn_back_->setSizePolicy(sizePolicy1);

    btn_next_ = new QPushButton(tr("Next>>"), this);
    sizePolicy1.setHeightForWidth(btn_next_->sizePolicy().hasHeightForWidth());
    btn_next_->setSizePolicy(sizePolicy1);


    lay_btn->addWidget(btn_snapshot);
    lay_btn->addWidget(btn_delete);
    lay_btn->addStretch();
    lay_btn->addWidget(btn_gt_prev_);
    lay_btn->addWidget(lab_gt_num_);
    lay_btn->addWidget(btn_gt_next_);
    lay_btn->addStretch();
    lay_btn->addWidget(btn_back_);
    lay_btn->addWidget(btn_next_);

    stackedwidget_ = new QStackedWidget(this);
    stackedwidget_->addWidget(widget_roi);
    stackedwidget_->addWidget(widget_pers);
    stackedwidget_->addWidget(widget_gt);

    layout->addWidget(stackedwidget_);
    layout->addLayout(lay_btn);

    setLayout(layout);

    // setup signal/slot connection
    connect(btn_snapshot, SIGNAL(clicked()), this, SLOT(OnBtnSnapshotClicked()));
    connect(btn_delete, SIGNAL(clicked()), this, SLOT(OnBtnDeleteClicked()));
    connect(btn_next_, SIGNAL(clicked()), this, SLOT(OnBtnNextClicked()));
    connect(btn_back_, SIGNAL(clicked()), this, SLOT(OnBtnBackClicked()));
    connect(btn_gt_next_, SIGNAL(clicked()), this, SLOT(OnBtnGTNextClicked()));
    connect(btn_gt_prev_, SIGNAL(clicked()), this, SLOT(OnBtnGTPrevClicked()));

    stackedwidget_->setCurrentIndex(0);
}

UCountingSetting::~UCountingSetting()
{

}

QVector<QPointF> UCountingSetting::roi_points()
{
    return sketchpad_roi_->points();
}

QVector<QPointF> UCountingSetting::pers_points()
{
    return sketchpad_pers_->points();
}

 QVector<QVector<QPointF> > UCountingSetting::gt_points()
{
     QVector<QVector<QPointF> > my_gt_points;
     for (int i=0; i<sketchpad_gt_.size(); ++i) {
         my_gt_points.append(sketchpad_gt_[i]->points());
     }

     return my_gt_points;
 }

 QVector<cv::Mat> UCountingSetting::gt_images()
 {
     return img_gt_;
 }

 void UCountingSetting::set_roi_points(cv::Mat img, QVector<QPointF> mypoints)
 {
     sketchpad_roi_->set_draw_finished(true);

     sketchpad_roi_->setPixmap(QPixmap::fromImage(QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888)));

     sketchpad_roi_->set_points(mypoints);

     UpdateGTSketchpadNum();
 }

 void UCountingSetting::set_pers_points(cv::Mat img, QVector<QPointF> mypoints)
 {
     sketchpad_pers_->set_draw_finished(true);

     sketchpad_pers_->setPixmap(QPixmap::fromImage(QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888)));

     sketchpad_pers_->set_points(mypoints);

     UpdateGTSketchpadNum();
 }

 void UCountingSetting::set_gt_points(QVector<cv::Mat> imgs, QVector<QVector<QPointF> > mypoints)
 {
     img_gt_ = imgs;

     for (int i=0; i<img_gt_.size(); ++i) {
         cv::Mat img = img_gt_[i];

         Sketchpad *sketchpad= new Sketchpad(this);
         sketchpad->set_curr_type(GTPoints);
         sketchpad->setFixedSize(kSketchpadWidth, kSketchpadHeight);
         sketchpad->ResetSketchpad();
         sketchpad->set_draw_finished(true);
         sketchpad->setPixmap(QPixmap::fromImage(QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888)));

         sketchpad->set_points(mypoints[i]);

         sketchpad_gt_.push_back(sketchpad);

         stackedwidget_gt_->addWidget(sketchpad);
         stackedwidget_gt_->setCurrentIndex(stackedwidget_gt_->count() - 1);
     }

     UpdateGTSketchpadNum();
 }

void UCountingSetting::Snapshot(cv::Mat img)
{
    if (stackedwidget_->currentIndex() == 0) {
        sketchpad_roi_->ResetSketchpad();
        sketchpad_roi_->setPixmap(QPixmap::fromImage(QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888)));

    } else if (stackedwidget_->currentIndex() == 1) {
        sketchpad_pers_->ResetSketchpad();
        sketchpad_pers_->setPixmap(QPixmap::fromImage(QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888)));

    } else {
        // save img and sketchpad
        img_gt_.push_back(img.clone());

        Sketchpad *sketchpad= new Sketchpad(this);
        sketchpad->set_curr_type(GTPoints);
        sketchpad->setFixedSize(kSketchpadWidth, kSketchpadHeight);
        sketchpad->ResetSketchpad();
        sketchpad->setPixmap(QPixmap::fromImage(QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888)));
        sketchpad_gt_.push_back(sketchpad);

        stackedwidget_gt_->addWidget(sketchpad);
        stackedwidget_gt_->setCurrentIndex(stackedwidget_gt_->count() - 1);
    }

    UpdateGTSketchpadNum();
}

void UCountingSetting::OnBtnSnapshotClicked()
{
    UpdateGTSketchpadNum();
    emit BtnSnapshotClicked();
}

void UCountingSetting::OnBtnDeleteClicked()
{
    if (stackedwidget_->currentIndex() == 0) {
        sketchpad_roi_->ResetSketchpad();

    } else if (stackedwidget_->currentIndex() == 1) {
        sketchpad_pers_->ResetSketchpad();

    } else {
        if (stackedwidget_gt_->count() == 0) {
            return;
        }

        // mark the index of current sketchpad
        int index = stackedwidget_gt_->currentIndex();

        // remove current sketchpad from stackedwidget_gt_
        stackedwidget_gt_->removeWidget(stackedwidget_gt_->currentWidget());

        // remove this sketchpad from sketchpad_gt_ and delete it
        Sketchpad *sketchpad = sketchpad_gt_.at(index);
        sketchpad_gt_.remove(index);
        delete sketchpad;

        // remove image of this sketchpad
        img_gt_.remove(index);

        UpdateGTSketchpadNum();
    }
}

void UCountingSetting::OnBtnNextClicked()
{
    if (stackedwidget_->currentIndex() == 0) {
        stackedwidget_->setCurrentIndex(stackedwidget_->currentIndex() + 1);
        btn_back_->setEnabled(true);
    } else if (stackedwidget_->currentIndex() == 1) {
        stackedwidget_->setCurrentIndex(stackedwidget_->currentIndex() + 1);
        btn_next_->setText(tr("Finish"));
        btn_gt_prev_->setHidden(false);
        lab_gt_num_->setHidden(false);
        btn_gt_next_->setHidden(false);
    } else {
        // counting setting finished, emit signal to delegate
        if (img_gt_.size() < 2) {
            QMessageBox::critical(NULL, "critical", "At Least Two!", QMessageBox::Ok, QMessageBox::Ok);
            return;
        }
        emit BtnFinishClicked();
    }
}

void UCountingSetting::OnBtnBackClicked()
{
    if (stackedwidget_->currentIndex() == 1) {
        stackedwidget_->setCurrentIndex(stackedwidget_->currentIndex() - 1);
        btn_back_->setEnabled(false);
    } else if (stackedwidget_->currentIndex() == 2) {
        stackedwidget_->setCurrentIndex(stackedwidget_->currentIndex() - 1);
        btn_next_->setText(tr("Next>>"));
        btn_gt_prev_->setHidden(true);
        lab_gt_num_->setHidden(true);
        btn_gt_next_->setHidden(true);
    }
}

void UCountingSetting::OnBtnGTNextClicked()
{
    if (stackedwidget_gt_->count() == 0)
        return;

    stackedwidget_gt_->setCurrentIndex(stackedwidget_gt_->currentIndex() + 1);

    UpdateGTSketchpadNum();

}

void UCountingSetting::OnBtnGTPrevClicked()
{
    if (stackedwidget_gt_->count() == 0)
        return;

    stackedwidget_gt_->setCurrentIndex(stackedwidget_gt_->currentIndex() - 1);

    UpdateGTSketchpadNum();
}

void UCountingSetting::UpdateGTSketchpadNum()
{
    lab_gt_num_->setText(QString("%1 / %2").arg(stackedwidget_gt_->currentIndex() + 1).arg(stackedwidget_gt_->count()));
}

