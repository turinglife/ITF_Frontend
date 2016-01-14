#include "countingsetting.h"
#include <QLayout>
#include <QAction>
#include <QToolBar>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QMessageBox>
#include <QSplitter>
#include <QCloseEvent>
#include "utility.h"
#include "usketchpadwidget.h"

CountingSetting::CountingSetting(string task_type, QWidget *parent)
    : QMainWindow(parent)
    , task_type_(task_type)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);

//! [CentralWidget]
    QWidget *centralwidget = new QWidget(this);
    setCentralWidget(centralwidget);

//! [StackedWidget]
    QVBoxLayout *p_layout = new QVBoxLayout(centralwidget);
    p_stackedwidget_main_ = new QStackedWidget(this);
    p_layout->addWidget(p_stackedwidget_main_);

//! [QSplitter]
    //ROI
    QSplitter *p_splitter_roi = new QSplitter(Qt::Horizontal, this);
    QLabel *p_guide_roi = new QLabel(this);
    p_guide_roi->setAlignment(Qt::AlignTop);
    p_guide_roi->setWordWrap(true);
    p_guide_roi->setMaximumWidth(300);
    if (task_type_ == kTaskTypeCounting) {
        p_guide_roi->setText(tr("Steps of Region of Interest (ROI)\n"
                              "\n"
                              "1. Click \"Clear\" to clear history records (if any);\n"
                              "2. Click \"Draw\" to enter DrawMode;\n"
                              "3. Click on the image to draw ROI;\n"
                              "3. Double click to exit DrawMode;\n"
                              "4. Click \"Next\";\n"
                              "\n"
                              "Notes: \n"
                              "* Click \"Right Mouse\" on image to undo last point when you are in DrawMode;\n"));
    } else if (task_type_ == kTaskTypeCrossline) {
        p_guide_roi->setText(tr("Steps of LINE of Interest (IOI)\n"
                              "\n"
                              "1. Click \"Clear\" to clear history records (if any);\n"
                              "2. Click \"Draw\" to enter DrawMode;\n"
                              "3. Click on the image to draw Line;\n"
                              "3. Double click to exit DrawMode;\n"
                              "4. Click \"Next\";\n"
                              "\n"
                              "Notes: \n"
                              "* Click \"Right Mouse\" on image to undo last point when you are in DrawMode;\n"));
    }
    p_sketchpad_roi_ = new USketchPadWidget(this);
    if (task_type_ == kTaskTypeCounting) {
        p_sketchpad_roi_->set_draw_type(ROI);
    } else if (task_type_ == kTaskTypeCrossline) {
        p_sketchpad_roi_->set_draw_type(LINE);
    }
    p_splitter_roi->addWidget(p_guide_roi);
    p_splitter_roi->addWidget(p_sketchpad_roi_);
    p_splitter_roi->setStretchFactor(0,0);
    p_splitter_roi->setStretchFactor(1,1);
    p_stackedwidget_main_->addWidget(p_splitter_roi);

////! [QSplitter]
    //Pers
    QSplitter *p_splitter_pers = new QSplitter(Qt::Horizontal, this);
    QLabel *p_guide_pers = new QLabel(this);
    p_guide_pers->setAlignment(Qt::AlignTop);
    p_guide_pers->setWordWrap(true);
    p_guide_pers->setMaximumWidth(300);
    p_guide_pers->setText(tr("Steps of Pesestrain Size\n"
                             "\n"
                             "1. Click \"Clear\" to clear history records (if any);\n"
                             "2. Click \"Snapshot\" to get a image which contains at least 2 pedestrains who have different distance from camera;\n"
                             "3. Click \"Draw\" to enter LabelMode;\n"
                             "4. Draw rectangle to enclose pedestrain;\n"
                             "5. Repeat step 4 to draw another pedestrain;\n"
                             "6. Double click image to exit LabelMode;\n"
                             "7. Click \"Next\";\n"
                             "\n"
                             "Notes: \n"
                             "* Click \"Right Mouse\" on image to undo last point when you are in DrawMode;\n"));
    p_sketchpad_pers_ = new USketchPadWidget(this);
    p_sketchpad_pers_->set_draw_type(PersMap);
    p_splitter_pers->addWidget(p_guide_pers);
    p_splitter_pers->addWidget(p_sketchpad_pers_);
    p_splitter_pers->setStretchFactor(0,1);
    p_splitter_pers->setStretchFactor(1,6);
    p_stackedwidget_main_->addWidget(p_splitter_pers);

//! [QSplitter]
    //GT
    QSplitter *p_splitter_gt = new QSplitter(Qt::Horizontal, this);
    QLabel *p_guide_gt = new QLabel(this);
    p_guide_gt->setAlignment(Qt::AlignTop);
    p_guide_gt->setWordWrap(true);
    p_guide_gt->setMaximumWidth(300);
    p_guide_gt->setText(tr("Steps of Label Pesestrains\n"
                         "\n"
                         "1. Click \"Snapshot\" to get a image\n"
                         "2. Click \"Delete\" to delete a image (if you don't like this image);\n"
                         "2. Click \"Clear\" to clear history records (if any);\n"
                         "3. Click \"Label\" to enter LabelMode;\n"
                         "4. Click pedestrains' head to label;\n"
                         "5. Double click image to exit LabelMode when all pesestrains' head in ROI are labeled';\n"
                         "6. Go through above steps to label another image;\n"
                         "7. Click \"Finish\" to finish;\n"
                         "\n"
                         "Notes: \n"
                         "* Click \"Right Mouse\" on image to undo last point when you are in LabelMode;\n"));
    p_stackedwidget_gt_ = new QStackedWidget(this);
    p_splitter_gt->addWidget(p_guide_gt);
    p_splitter_gt->addWidget(p_stackedwidget_gt_);
    p_splitter_gt->setStretchFactor(0, 0);
    p_splitter_gt->setStretchFactor(1, 1);
    p_stackedwidget_main_->addWidget(p_splitter_gt);

    connect(p_sketchpad_roi_, SIGNAL(SketchPadFinished()), this, SLOT(OnSketchPadFinished()));
    connect(p_sketchpad_pers_, SIGNAL(SketchPadFinished()), this, SLOT(OnSketchPadFinished()));

    p_stackedwidget_main_->setCurrentIndex(0);

//! [Action] [TOOLBAR]
    p_snapshot_ = new QAction(tr("Snapshot"), this);
    p_draw_ = new QAction(tr("Draw"), this);
    p_clear_ = new QAction(tr("Clear"), this);
    p_delete_ = new QAction(tr("Delete"), this);

    connect(p_snapshot_, SIGNAL(triggered()), this, SLOT(OnActionSnapshotTriggered()));
    connect(p_draw_, SIGNAL(triggered()), this, SLOT(OnActionDrawTriggered()));
    connect(p_clear_, SIGNAL(triggered()), this, SLOT(OnActionClearTriggered()));
    connect(p_delete_, SIGNAL(triggered()), this, SLOT(OnActionDeleteTriggered()));

    p_toolbar_ = addToolBar("Action");
    p_toolbar_->addAction(p_snapshot_);
    p_toolbar_->addAction(p_draw_);
    p_toolbar_->addAction(p_clear_);
    p_toolbar_->addAction(p_delete_);

//! [QPushButton]
    QHBoxLayout *p_lay_btn = new QHBoxLayout;
    p_back_ = new QPushButton(tr("<<Back"), this);
    p_next_ = new QPushButton(tr("Next>>"), this);
    p_gt_prev_ = new QPushButton(tr("<"), this);
    p_gt_num_ = new QLabel(this);
    p_gt_next_ = new QPushButton(tr(">"),this);

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

    setWindowTitle("Region of Interest");
    set_btn_visible(true, false, true, false, false, true, false, false, false);
}

CountingSetting::~CountingSetting()
{
    std::cout << "release CountingSetting" << std::endl;
    release_memory();
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

// Init ROI from history
void CountingSetting::set_roi_points_images(cv::Mat img, QVector<QPointF> pts)
{
    p_sketchpad_roi_->set_draw_finished(true);

    p_sketchpad_roi_->ShowImage(img); /// ShowImage need to process before set_points, since set_points need images' rows and cols;

    p_sketchpad_roi_->set_points(pts);
}

// Init Pers from history
void CountingSetting::set_pers_points_images(cv::Mat img, QVector<QPointF> pts)
{
    p_sketchpad_pers_->set_draw_finished(true);

    p_sketchpad_pers_->ShowImage(img); /// ShowImage need to process before set_points, since set_points need images' rows and cols;

    p_sketchpad_pers_->set_points(pts);
}

//// Init GT from history
void CountingSetting::set_gt_points_images(QVector<cv::Mat> imgs, QVector<QVector<QPointF> > pts)
{
    img_gt_ = imgs;

    for(int i=0; i<img_gt_.size(); ++i) {
        USketchPadWidget *p_sketchpad = new USketchPadWidget(this);
        p_sketchpad->set_draw_type(GTPoints);
        p_sketchpad->ShowImage(img_gt_[i]); /// ShowImage need to process before set_points, since set_points need images' rows and cols;
        p_sketchpad->set_points(pts[i]);
        sketchpad_gt_.push_back(p_sketchpad);

        p_stackedwidget_gt_->addWidget(sketchpad_gt_.last());
        p_stackedwidget_gt_->setCurrentIndex(p_stackedwidget_gt_->count() - 1);

        connect(sketchpad_gt_.last(), SIGNAL(SketchPadFinished()), this, SLOT(OnSketchPadFinished()));
    }

    UpdateGTNum();
}

void CountingSetting::release_memory()
{
    std::cout << "release sketchpad_gt_" << std::endl;
    while (!sketchpad_gt_.isEmpty()) {
        USketchPadWidget *p_sketchpad = sketchpad_gt_.last();
        sketchpad_gt_.pop_back();
        p_sketchpad->deleteLater();
    }

    std::cout << "release sketchpad_pers_" << std::endl;
    p_sketchpad_pers_->deleteLater();
    std::cout << "release sketchpad_roi_" << std::endl;
    p_sketchpad_roi_->deleteLater();
}

void CountingSetting::set_btn_visible(bool flag_draw, bool flag_snapshot, bool flag_clear, bool flag_delete, bool flag_back, bool flag_next, bool flag_gt_prev, bool flag_gt_next, bool flag_gt_num)
{
   /* draw, snapshot, clear, delete, back, next, gt_prev, gt_next, gt_num */
    p_draw_->setVisible(flag_draw);
    p_snapshot_->setVisible(flag_snapshot);
    p_clear_->setVisible(flag_clear);
    p_delete_->setVisible(flag_delete);
    p_back_->setVisible(flag_back);
    p_next_->setVisible(flag_next);
    p_gt_prev_->setVisible(flag_gt_prev);
    p_gt_next_->setVisible(flag_gt_next);
    p_gt_num_->setVisible(flag_gt_num);
}

void CountingSetting::ImageWithROI(cv::Mat &img)
{
    QVector<QPointF> roi_pts = p_sketchpad_roi_->points();
    for (int i=0; i<roi_pts.size()-1; ++i) {
        cv::line(img, cvPoint(roi_pts[i].x(), roi_pts[i].y()), cvPoint(roi_pts[i+1].x(), roi_pts[i+1].y()), cv::Scalar( 0, 0, 255),  2, 8 );
    }
    cv::line(img, cvPoint(roi_pts.first().x(), roi_pts.first().y()), cvPoint(roi_pts.last().x(), roi_pts.last().y()), cv::Scalar( 0, 0, 255),  2, 8 );
}

void CountingSetting::Snapshot(cv::Mat img)
{
    if (p_stackedwidget_main_->currentIndex() == 0) {
//        p_sketchpad_roi_->clear_sketchpad();
        p_sketchpad_roi_->ShowImage(img);
    } else if (p_stackedwidget_main_->currentIndex() == 1) {
//        p_sketchpad_pers_->clear_sketchpad();
        p_sketchpad_pers_->ShowImage(img);
    } else if (p_stackedwidget_main_->currentIndex() == 2) {
        if (img_gt_.empty()) {
            UpdateBtnStatus(true);
        }
        img_gt_.push_back(img.clone());
        USketchPadWidget *p_sketchpad = new USketchPadWidget(this);
        p_sketchpad->set_draw_type(GTPoints);
        // draw roi on image
        ImageWithROI(img);
        p_sketchpad->ShowImage(img);
        sketchpad_gt_.push_back(p_sketchpad);

        p_stackedwidget_gt_->addWidget(sketchpad_gt_.last());
        p_stackedwidget_gt_->setCurrentIndex(p_stackedwidget_gt_->count() - 1);
        connect(sketchpad_gt_.last(), SIGNAL(SketchPadFinished()), this, SLOT(OnSketchPadFinished()));
    } else {
        return;
    }

    UpdateGTNum();
}

void CountingSetting::closeEvent(QCloseEvent *event)
{
    if (QMessageBox::No == QMessageBox::question(NULL, "Information", "Discard all changes?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No)) {
        event->ignore();
        return;
    }
}

void CountingSetting::OnActionDrawTriggered()
{
    setCursor(Qt::CrossCursor);
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

// only vailable for gt
void CountingSetting::OnActionDeleteTriggered()
{
    if (p_stackedwidget_main_->currentIndex() != 2 || p_stackedwidget_gt_->count() == 0)
        return;
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
        UpdateBtnStatus(false);
        p_snapshot_->setEnabled(true);
        p_back_->setEnabled(true);
//        emit ActionSnapshotTriggered();
    }
}

void CountingSetting::OnBtnBackClicked()
{
    if (p_stackedwidget_main_->currentIndex() == 1) {
        p_stackedwidget_main_->setCurrentIndex(0);
        if (p_sketchpad_pers_->points().empty()) {
            this->setWindowTitle(tr("Line of Interest"));
            p_next_->setText(tr("Next>>"));
        } else {
            this->setWindowTitle(tr("Region of Interest"));
        }
        set_btn_visible(true, false, true, false, false, true, false, false, false);
    } else if (p_stackedwidget_main_->currentIndex() == 2) {
        p_stackedwidget_main_->setCurrentIndex(1);
        this->setWindowTitle(tr("Human Size"));
        p_next_->setText(tr("Next>>"));
        p_draw_->setText(tr("Draw"));
        set_btn_visible(true, true, true, false, true, true, false, false, false);
    } else {
        return;
    }
}

void CountingSetting::OnBtnNextClicked()
{
    if (p_stackedwidget_main_->currentIndex() == 0) {
        if (p_sketchpad_roi_->points().isEmpty()) {
            QMessageBox::information(NULL, "Setting", "Region of Interest is require!", QMessageBox::Ok, QMessageBox::Ok);
            return;
        }
        p_stackedwidget_main_->setCurrentIndex(1);
        this->setWindowTitle(tr("Human Size"));
        set_btn_visible(true, true, true, false, true, true, false, false, false);
        if (task_type_ == kTaskTypeCrossline) {
            p_next_->setText(tr("Finish"));
        }

        // if p_sketchpad_pers_'s points is empty, then need snapshot
        if (p_sketchpad_pers_->points().empty()) {
            emit ActionSnapshotTriggered();
        }
    } else if (p_stackedwidget_main_->currentIndex() == 1) {
        if (p_sketchpad_pers_->points().size()/2 < 2) {
            QMessageBox::information(NULL, "Setting", "Human Size at least two rectangles!", QMessageBox::Ok, QMessageBox::Ok);
            return;
        }
        if (task_type_ == kTaskTypeCrossline) {
            emit CountingSettingFinished();
            return;
        }

        p_stackedwidget_main_->setCurrentIndex(2);
        this->setWindowTitle(tr("Label People"));
        p_next_->setText(tr("Finish"));
        p_draw_->setText(tr("Label"));
        set_btn_visible(true, true, true, true, true, true, true, true, true);
        if (img_gt_.empty()) {
            UpdateBtnStatus(false);
            p_snapshot_->setEnabled(true);
            p_back_->setEnabled(true);
        }

        // draw ROI on image
        for (int i=0; i<img_gt_.size(); ++i) {
            cv::Mat img = img_gt_[i].clone();
            ImageWithROI(img);
            sketchpad_gt_[i]->ShowImage(img);
        }

//        // if gt is empty, then need snapshot
//        if (img_gt_.empty()) {
//            emit ActionSnapshotTriggered();
//        }
    } else if (p_stackedwidget_main_->currentIndex() == 2) {
        // counting setting finished, emit signal to delegate
        if (sketchpad_gt_.size() < 2) {
            QMessageBox::information(NULL, "Setting", "Please snapshot at least two Images!", QMessageBox::Ok, QMessageBox::Ok);
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
    setCursor(Qt::ArrowCursor);
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
