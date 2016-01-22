#include "delegate.h"
#include "utility.h"
#include <QTime>
#include <math.h>

Delegate::Delegate(QWidget *parent)
    : QWidget(parent)
    , alarm_started_(false)
    , video_finished_(false)
    , curr_count_1_(0)
    , curr_count_2_(0)
{
    fps_ = 20;
}

Delegate::~Delegate()
{
    std::cout << "release Delegate begin" << std::endl;
    release_memory();
    std::cout << "release Delegate Done" << std::endl;
}

void Delegate::Init(std::vector<std::map<string, string> > task_info, int channel)
{
//! [Init Parameters]
    task_info_ = task_info[0];
    curr_channel_ = channel;

//! Take care of Wrong Case
    if (task_info_["camera_status"] == kStatusOFF && task_info_["task_status"] == kStatusON) {
        task_info_["task_status"] = kStatusOFF;
        if (!DBHelper::UpdateStatusToTasks("task_status", task_info_["task_status"], task_info_["task_name"])) {
            QMessageBox::warning(0, "Warning", "Update DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
            return;
        }
    }

//! [Create new thread for execute command]
    p_command_center_ = new TaskCommandCenter(task_info_["task_name"], 0);
    p_command_center_->moveToThread(&thread_);

    std::cout << "clip" << std::endl;
//! [Create and Setup clipwidget for src and dst]
    p_src_clip_ = new UClipWidget(this);
    p_dst_clip_1_ = new UClipWidget(this);
    p_dst_clip_2_ = new UClipWidget(this);
    Utility::SetLabelStyle(p_src_clip_);
    Utility::SetLabelStyle(p_dst_clip_1_);
    Utility::SetLabelStyle(p_dst_clip_2_);
    p_src_clip_->set_curr_name(task_info_["task_name"]);
    p_src_clip_->set_curr_type(task_info_["task_type"]);
    p_src_clip_->setVisible(false);
    p_dst_clip_1_->setVisible(false);
    p_dst_clip_2_->setVisible(false);

    std::cout << "plot" << std::endl;
//! [Create Plot]
    p_plot_ = new UPlotWidget(this);
    p_plot_->clear_plot();
    // currently view video as camera
    double curr_datetime = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    p_plot_->set_plot(curr_datetime, curr_datetime+90.0, kCameraTypeHttp, task_info_["task_type"]);
    if (task_info_["task_type"] == kTaskTypeCounting) {
//        Utility::SetWdgSizePolicy(p_plot_, QSizePolicy::Ignored, QSizePolicy::Ignored);
        Utility::SetWdgSizePolicy(p_plot_, QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
    p_plot_->setVisible(false);

    std::cout << "alarm" << std::endl;
//! [Alarm]
    p_clip_alarm_ = new UClipAlarmWidget(this);
    p_clip_alarm_->setVisible(false);
    std::vector<std::map<std::string, std::string> > res_alarm_strategy;
    if (!DBHelper::SelectFromTable("DensityAlarmStrategy", "task_name", task_info_["task_name"], res_alarm_strategy))
        return;
    if (res_alarm_strategy.empty()) {
        std::string priority = "0";
        p_clip_alarm_->set_alarm_strategy(false, priority, priority, priority);
    } else {
        p_clip_alarm_->set_alarm_strategy(true, res_alarm_strategy[0]["priority_low"], res_alarm_strategy[0]["priority_medium"], res_alarm_strategy[0]["priority_high"]);
    }

    std::cout << "connection" << std::endl;
//! [Setup Connecton]
    connect(p_command_center_, SIGNAL(CommandExecuted(QString,QString,bool)), this, SLOT(MessageReceiver(QString,QString,bool)));
    connect(this, SIGNAL(CameraStoped()), p_command_center_, SLOT(deleteLater()));
    connect(this, SIGNAL(CameraStoped()), &thread_, SLOT(quit()));
    connect(&thread_, SIGNAL(started()), this, SLOT(OnThreadStarted()));
    connect(&thread_, SIGNAL(finished()), this, SLOT(OnThreadFInished()));
    connect(this, SIGNAL(CameraStarted()), this, SLOT(OnCameraStarted()));
    connect(this, SIGNAL(TaskStarted()), this, SLOT(OnTaskStarted()));
    connect(this, SIGNAL(TaskStoped()), this, SLOT(OnTaskStoped()));
    connect(p_src_clip_, SIGNAL(StartAD()), this, SLOT(OnStartADClicked()));
    connect(p_src_clip_, SIGNAL(StopAD()), this, SLOT(OnStopADclicked()));
    connect(p_src_clip_, SIGNAL(RemoveFromBoard()), this, SLOT(OnRemoveFromBoardClicked()));
    connect(p_src_clip_, SIGNAL(Setting()), this, SLOT(OnCountingSettingClicked()));
    connect(p_src_clip_, SIGNAL(ShowClipAnalysisBoard()), this, SLOT(OnShowClipAnalysisBoardClicked()));
    connect(&timer_src_, SIGNAL(timeout()), this, SLOT(PlaySrc()));
    connect(&timer_dst_, SIGNAL(timeout()), this, SLOT(PlayDst()));

//! [Start Thread]
    thread_.start();
}

UClipWidget *Delegate::src_clip()
{
    return p_src_clip_;
}

UClipWidget *Delegate::dst_clip_1() const
{
    return p_dst_clip_1_;
}
UClipWidget *Delegate::dst_clip_2() const
{
    return p_dst_clip_2_;
}

UPlotWidget *Delegate::plot() const
{
    return p_plot_;
}

UClipAlarmWidget *Delegate::clip_alarm()
{
    return p_clip_alarm_;
}

void Delegate::StartCD()
{
    CommandAssigner(QString::fromStdString(kDaemonCD), QString::fromStdString(kCommandStart));
}

void Delegate::StopCD()
{
    CommandAssigner(QString::fromStdString(kDaemonCD), QString::fromStdString(kCommandStop));
}

void Delegate::StartAD()
{
    CommandAssigner(QString::fromStdString(kDaemonAD), QString::fromStdString(kCommandStart));
}

void Delegate::StopAD()
{
    CommandAssigner(QString::fromStdString(kDaemonAD), QString::fromStdString(kCommandStop));
}
void Delegate::StartRD()
{
    CommandAssigner(QString::fromStdString(kDaemonRD), QString::fromStdString(kCommandStart));
}

void Delegate::StartMD()
{
    CommandAssigner(QString::fromStdString(kDaemonMD), QString::fromStdString(kCommandStart));
}

void Delegate::StopMD()
{
    CommandAssigner(QString::fromStdString(kDaemonMD), QString::fromStdString(kCommandStop));
}

void Delegate::PlaySrc()
{
    std::cout << "Begin PlaySrc" << std::endl;
    if (!buffer_.fetch_src(curr_timestamp_, curr_src_, false)) {
        if (!buffer_.is_camera_valid()) {
            /// Video is over
            VideoIsFinished();
        }
        return;
    }
    cv::Mat img_src = curr_src_.clone();
    /* Show ROI and Counting result */
    if (timer_dst_.isActive()) {
        if (task_info_["task_type"] == kTaskTypeCounting) {
            for (int i=0; i<roi_points_.size()-1; ++i) {
                cv::line(img_src, roi_points_[i], roi_points_[i+1], cv::Scalar(100, 0, 255),  3, 8 );
            }
            cv::line(img_src, roi_points_.first(), roi_points_.last(), cv::Scalar(100, 0, 255),  3, 8 );
            cv::putText(img_src, std::to_string(curr_count_1_), cv::Point(50, 60), cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 2, cv::Scalar(0, 100, 255), 3, 8);
        } else if (task_info_["task_type"] == kTaskTypeCrossline){
            cv::line(img_src, roi_points_[0], roi_points_[1], cv::Scalar(100, 0, 255),  3, 8 );

            ArrowedLine(img_src, arrow_points_[0], arrow_points_[1], cv::Scalar(100, 255, 0),  3, 8);
            cv::putText(img_src, std::to_string(curr_count_1_), cv::Point(arrow_points_[1].x + 50, arrow_points_[0].y + 50), cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 2, cv::Scalar(100, 255, 0), 3, 8);

            ArrowedLine(img_src, arrow_points_[3], arrow_points_[2], cv::Scalar(0, 100, 255),  3, 8);
            cv::putText(img_src, std::to_string(curr_count_2_), cv::Point(arrow_points_[3].x + 50, arrow_points_[2].y + 50), cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 2, cv::Scalar(0, 100, 255), 3, 8);
        }
    }

    p_src_clip_->ShowImage(img_src);
    std::cout << "End PlaySrc" << std::endl;
}

void Delegate::PlayDst()
{
    std::cout << "Begin PlayDst" << std::endl;
    if (task_info_["task_type"] == kTaskTypeCounting) {
        if(!buffer_.fetch_dst(curr_timestamp_, curr_dst_1_, curr_count_1_, false)) {
            return;
        }
        /* Plot, view video as camera */
        double key;
        if (task_info_["camera_type"] == kCameraTypeFile) {
            key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
        } else {
            key = static_cast<double> (curr_timestamp_);
        }
        p_plot_->set_data(key, curr_count_1_);
        /* Alarm */
        p_clip_alarm_->set_value(curr_count_1_);
        /* dst clip 1 */
        p_dst_clip_1_->ShowImage(curr_dst_1_);

    } else if (task_info_["task_type"] == kTaskTypeCrossline) {
        if(!buffer_.fetch_dst(curr_timestamp_,  curr_dst_1_, curr_dst_2_, curr_count_1_, curr_count_2_, false)) {
            return;
        }
        double key;
        if (task_info_["camera_type"] == kCameraTypeFile) {
            key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
        } else {
            key = static_cast<double> (curr_timestamp_);
        }

        p_plot_->set_data(key, curr_count_1_+curr_count_2_, curr_count_1_, curr_count_2_);

//        /*convert to slice*/
//        std::cout << "Slice" << std::endl;
        ConvertToSlice(curr_dst_1_, curr_dst_slice_1_, roi_points_[0], roi_points_[1]);
        ConvertToSlice(curr_dst_2_, curr_dst_slice_2_, roi_points_[0], roi_points_[1]);
        /* dst clip */
        p_dst_clip_1_->ShowImage(curr_dst_slice_1_);
        p_dst_clip_2_->ShowImage(curr_dst_slice_2_);
//        p_dst_clip_1_->ShowImage(curr_dst_1_);
//        p_dst_clip_2_->ShowImage(curr_dst_2_);

    } else {
        if(!buffer_.fetch_dst(curr_timestamp_, curr_dst_1_, false)) {
            return;
        }
        /* dst clip 1 */
        p_dst_clip_1_->ShowImage(curr_dst_1_);
    }
    std::cout << "End PlayDst" << std::endl;
}

void Delegate::CommandAssigner(QString daemon, QString operation)
{
    qDebug() << daemon << " " << operation;
    /* Start CD */
    if (daemon == QString::fromStdString(kDaemonCD) && operation == QString::fromStdString(kCommandStart)) {
        p_src_clip_->set_clip_controler(false, false, false, false, false);
//        ConnectStatus(1);
    }

    /* Stop CD */
     if (daemon == QString::fromStdString(kDaemonCD) && operation == QString::fromStdString(kCommandStop)) {
        p_src_clip_->set_clip_controler(false, false, false, false, false);
        task_info_["camera_status"] = kStatusOFF;
        if (! DBHelper::UpdateStatusToTasks("camera_status", task_info_["camera_status"], task_info_["task_name"])) {
            QMessageBox::warning(0, "Warning", "Update DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
        }
     }

    /* Start AD */
     if (daemon == QString::fromStdString(kDaemonAD) && operation == QString::fromStdString(kCommandStart)) {
         p_src_clip_->set_clip_controler(false, false, false, false, false);

         if (!IsReadyToStartAD()) {
             QMessageBox::information(NULL, "warning", "Please Setup Task!", QMessageBox::Ok, QMessageBox::Ok);
             p_src_clip_->set_clip_controler(true, true, false, false, true);
             return;
         }
     }

    /* Stop AD */
     if (daemon == QString::fromStdString(kDaemonAD) && operation == QString::fromStdString(kCommandStop)) {
         p_src_clip_->set_clip_controler(false, false, false, false, false);

         task_info_["task_status"] = kStatusOFF;
         if (!DBHelper::UpdateStatusToTasks("task_status", task_info_["task_status"], task_info_["task_name"])) {
             QMessageBox::warning(0, "Warning", "Update DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
         }
     }

     /* Start MD: Only AD Started will call StartMD() */
      if (daemon == QString::fromStdString(kDaemonMD) && operation == QString::fromStdString(kCommandStart)) {
          if (alarm_started_)
              return;
          p_src_clip_->set_clip_controler(false, false, false, false, false);
      }

     /* Stop MD: Only AD Stoped will call StopMD() */
      if (daemon == QString::fromStdString(kDaemonMD) && operation == QString::fromStdString(kCommandStop)) {
          if (!alarm_started_)
              return;
          alarm_started_ = false;
          p_src_clip_->set_clip_controler(false, false, false, false, false);
      }

     /* Processing Command in another thread */
    if (operation == QString::fromStdString(kCommandStart)) {
        QMetaObject::invokeMethod(p_command_center_, "OpenTask", Qt::QueuedConnection, Q_ARG(QString, daemon), Q_ARG(QString, operation));
    } else {
        QMetaObject::invokeMethod(p_command_center_, "ExecuteCommand", Qt::QueuedConnection, Q_ARG(QString, daemon), Q_ARG(QString, operation));
    }
}

void Delegate::MessageReceiver(QString daemon, QString operation, bool flag)
{
    qDebug() << daemon << " " << operation << " " << flag;
    /* CD */
    if (daemon == QString::fromStdString(kDaemonCD)) {
        /*START*/
        if (operation == QString::fromStdString(kCommandStart)) {
            if (flag) {
                /* CD START SUCCESS */
                if (InitBuffer()) {
                    p_src_clip_->set_clip_controler(true, true, false, false, true);
                    //-- Update camera_status==ON to DB */
                    task_info_["camera_status"] = kStatusON;
                    if (!DBHelper::UpdateStatusToTasks("camera_status", task_info_["camera_status"], task_info_["task_name"])) {
                        QMessageBox::warning(0, "Warning", "Update DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
                    }
                    emit CameraStarted();
                } else {
                    p_src_clip_->set_clip_controler(true, false, false, false, false);
//                    ConnectStatus(0);
                    //-- Update camera_status==OFF to DB */
                    task_info_["camera_status"] = kStatusOFF;
                    if (!DBHelper::UpdateStatusToTasks("camera_status", task_info_["camera_status"], task_info_["task_name"])) {
                        QMessageBox::warning(0, "Warning", "Update DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
                    }
                }
            } else {
                // CD START FAILED
                p_src_clip_->set_clip_controler(true, false, false, false, false);
//                ConnectStatus(0);
            }
        }
        /*STOP*/
        if (operation == QString::fromStdString(kCommandStop)){
            p_src_clip_->set_clip_controler(true, false, false, false, false);
            emit CameraStoped();
        }
    }

    /* AD */
    if (daemon == QString::fromStdString(kDaemonAD)) {
        /* START */
        if (operation == QString::fromStdString(kCommandStart)) {
            if (flag) {
                /* AD START SUCCESS*/
                p_src_clip_->set_clip_controler(false, false, true, true, false);
                /* -- Update task_status==ON to DB */
                task_info_["task_status"] = kStatusON;
                if (! DBHelper::UpdateStatusToTasks("task_status", task_info_["task_status"], task_info_["task_name"])) {
                    QMessageBox::warning(0, "Warning", "Update DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
                }

                emit TaskStarted();
            } else {
                /* AD START FAILED */
                p_src_clip_->set_clip_controler(true, true, false, false, true);
                /* -- Update task_status==OFF to DB */
                task_info_["task_status"] = kStatusOFF;
                if (! DBHelper::UpdateStatusToTasks("task_status", task_info_["task_status"], task_info_["task_name"])) {
                    QMessageBox::warning(0, "Warning", "Update DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
                }
            }
        }
        /* STOP */
        if (operation == QString::fromStdString(kCommandStop)){
            p_src_clip_->set_clip_controler(true, true, false, false, true);
            emit TaskStoped();
        }
    }

    /* RD : Special Case */
    if (daemon == QString::fromStdString(kDaemonRD)) {
        /* START */
        if (operation == QString::fromStdString(kCommandStart)) {
            if (flag) {
                /* RD START SUCCESS */
            } else {
               /* RD START FAILED */
                QMessageBox::warning(NULL, "warning", "RD START Fail! ", QMessageBox::Ok, QMessageBox::Ok);
            }
        }
    }

    /* MD */
    if (daemon == QString::fromStdString(kDaemonMD)) {
        /* START */
        if (operation == QString::fromStdString(kCommandStart)) {
            if (flag) {
                /* MD START SUCCESS */
                alarm_started_ = true;
            }
            p_src_clip_->set_clip_controler(false, false, true, true, false);
        }
        /* STOP */
        if (operation == QString::fromStdString(kCommandStop)){
            p_src_clip_->set_clip_controler(true, true, false, false, true);
        }
    }

}

void Delegate::OnCameraStarted()
{
    timer_src_.start(1000/fps_);

    if (task_info_["task_status"] == kStatusON) {
        // startAD when task_status is ON in DB
        StartAD();
    }
}

void Delegate::OnTaskStarted()
{
    timer_dst_.start(1000/fps_);

    if (task_info_["task_type"] == kTaskTypeCounting && task_info_["alarm_switch"] == kStatusON) {
        StartMD();
    }
}

void Delegate::OnTaskStoped()
{
    if (task_info_["task_type"] == kTaskTypeCounting && task_info_["alarm_switch"] == kStatusON) {
        StopMD();
    }
}

void Delegate::OnThreadStarted()
{
    if (timer_src_.isActive()) {
        return;
    }
    StartCD();
}

void Delegate::OnRemoveFromBoardClicked()
{
    if (timer_src_.isActive()) {
        timer_src_.stop();
        StopCD();
    } else {
        emit RemoveFromChannel(curr_channel_);
    }
}


void Delegate::OnThreadFInished()
{
    if (!video_finished_) {
        emit RemoveFromChannel(curr_channel_);
    }
}

void Delegate::OnStartADClicked()
{
    if (timer_dst_.isActive()) {
        return;
    }
    StartAD();
}

void Delegate::OnStopADclicked()
{
    if (timer_dst_.isActive()) {
        timer_dst_.stop();
        StopAD();
    }
}

void Delegate::OnShowClipAnalysisBoardClicked()
{
    if (timer_src_.isActive() && timer_dst_.isActive()) {
        emit ShowClipAnalysisBoard(curr_channel_);
    }
}

void Delegate::OnCountingSettingClicked()
{
//! [p_countingsetting]
    p_countingsetting_ = new CountingSetting(task_info_["task_type"], 0);

    // resize counting setting window
    p_countingsetting_->setMinimumSize(kSketchpadWidth, kSketchpadHeight);
    Utility::MoveToCenter(p_countingsetting_);
    p_countingsetting_->show();

    if (!ReadCountingSetting()) {
        p_countingsetting_->Snapshot(curr_src_);
    }

    connect(p_countingsetting_, &CountingSetting::ActionSnapshotTriggered, [=](){
        p_countingsetting_->Snapshot(curr_src_);
    });

    connect(p_countingsetting_, &CountingSetting::CountingSettingFinished, [=](){
        WriteCountingSetting();
        p_countingsetting_->deleteLater();
    });
}


bool Delegate::InitBuffer()
{
    if (!buffer_.Init(task_info_["task_name"])) {
        qDebug() << "Init Buffer failed";
        return false;
    }
    buffer_.frame_size(frame_width_, frame_height_);
    // Init curr_src_, curr_dst_1_ and curr_dst_2_, frame_ratio_
    curr_src_ = cv::Mat::zeros(frame_height_, frame_width_, CV_8UC3);
    curr_dst_1_ = cv::Mat::zeros(frame_height_, frame_width_, CV_8UC3);
    curr_dst_2_ = cv::Mat::zeros(frame_height_, frame_width_, CV_8UC3);
    frame_ratio_ = float(frame_width_) / float(frame_height_);

    return true;
}

bool Delegate::IsReadyToStartAD()
{
    if (task_info_["task_type"] == kTaskTypeSegmentation || task_info_["task_type"] == kTaskTypeStationary) {
        return true;
    }

    std::vector<std::map<std::string, std::string> > res;
    if (!DBHelper::SelectFromTable("DensityDetail", "task_name", task_info_["task_name"], res)) {
        return false;
    }
    if (res.empty()) {
        return false;
    }

    std::string pers_file = res[0]["pers_file"];
    std::string roi_file = res[0]["roi_file"];
    std::string lm_file = res[0]["lm_file"];
    if (pers_file.empty() || roi_file.empty() || lm_file.empty()) {
        return false;
    }

    /* Load ROI Points so that we could display ROI When Counting Task is processing */
    std::string roi_path = task_info_["task_path"] + kROIDir + roi_file;
    roi_points_.clear();

    if (!Utility::LoadPoints(roi_points_, roi_path)) {
        return false;
    }

    if (task_info_["task_type"] == kTaskTypeCrossline) {
        vector<cv::Vec3b> line;
        ExtractLine(curr_src_.clone(), roi_points_[0], roi_points_[1], line);
        slice_height_ = line.size() - 1;
        slice_width_ = slice_height_*frame_ratio_;
        // init slice mat
        curr_dst_slice_1_ = cv::Mat::zeros(slice_height_, slice_width_, CV_8UC3);
        curr_dst_slice_2_ = cv::Mat::zeros(slice_height_, slice_width_, CV_8UC3);

        // Init arrow position
        cv::Point pt1, pt2, pt3, pt4;
        int arrow_length = 80;
        pt1 = cv::Point(50, 10);
        OrthogonalLine(roi_points_[0], roi_points_[1], pt1, pt2, arrow_length);
        pt3 = cv::Point(pt1.x, pt1.y + arrow_length + 20);
        pt4 = cv::Point(pt2.x, pt2.y + arrow_length + 20);
        arrow_points_.clear();
        arrow_points_.push_back(pt1);
        arrow_points_.push_back(pt2);
        arrow_points_.push_back(pt3);
        arrow_points_.push_back(pt4);
    }

    return true;
}

bool Delegate::VideoIsFinished()
{
    std::cout << "video is finished" << std::endl;
    video_finished_ = true;
    // When Video is Finished, stop AD, CD
    if (timer_dst_.isActive()) {
        timer_dst_.stop();
        StopAD();
    }
    p_src_clip_->set_clip_controler(true, false, false, false, false);

    int index=0;
    while (++index < 5) {
        if (alarm_started_) {
            sleep(1);
        } else {
            break;
        }
    }

    OnRemoveFromBoardClicked();

    cv::Mat  img = cv::Mat::zeros(frame_height_, frame_width_, CV_8UC3);
    cv::putText(img, "Finished", cv::Point(frame_width_/2 - 100, frame_height_/2), cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 2, cv::Scalar(0, 100, 255), 5, 16);
    p_src_clip_->ShowImage(img);
    p_dst_clip_1_->ShowImage(img);
    p_dst_clip_2_->ShowImage(img);
    p_plot_->clear_plot();

    return true;
}

bool Delegate::ReadCountingSetting()
{
    // Select From DB
    std::vector<std::map<std::string, std::string> > res;
    if (!DBHelper::SelectFromTable("DensityDetail", "task_name", task_info_["task_name"], res)) {
        return false;
    }
    if (res.empty())
        return false;
    std::string roi_path = task_info_["task_path"] + kROIDir + res[0]["roi_file"];
    std::string pers_path= task_info_["task_path"] + kPersDir + res[0]["pers_file"];
//    std::string gt_dir = task_info_["task_path"] + kGTDir;

    QVector<QPointF> roi_points;
    QVector<QPointF> pers_points;
//    QVector<QVector<QPointF> > gt_points;
//    QVector<cv::Mat> gt_images;

    // Load ROI Points for Sketchpad
    if (!Utility::LoadPoints(roi_points, roi_path)) {
        return false;
    }
    p_countingsetting_->set_roi_points_images(curr_src_, roi_points);

    // Load Pers Points for Sketchpad
    if (!Utility::LoadPoints(pers_points, pers_path)) {
        return false;
    }
    p_countingsetting_->set_pers_points_images(curr_src_, pers_points);

//    /* get img and label data from gt_dir */
//    if (!Utility::ReadGTData(gt_dir, gt_points, gt_images)) {
//        return false;
//    }
//    p_countingsetting_->set_gt_points_images(gt_images, gt_points);

    return true;
}

void Delegate::WriteCountingSetting()
{
    // get roi_path, pers_path and gt_dir
    std::string roi_path = task_info_["task_path"] + kROIDir + kROIFile;
    std::string pers_path = task_info_["task_path"] + kPersDir + kPersFile;
    std::string gt_dir = task_info_["task_path"] + kGTDir;

    /* save roi_file and pers_file from Sketchpad*/
    if (!Utility::SavePoints(p_countingsetting_->roi_points(), roi_path)) {
        return;
    }
    if (!Utility::SavePoints(p_countingsetting_->pers_points(), pers_path)) {
        return;
    }

    if (task_info_["task_type"] == kTaskTypeCounting) {
        /*save gt data*/
        if (!Utility::WriteDGData(gt_dir, p_countingsetting_->gt_points(), p_countingsetting_->gt_images())) {
            return;
        }
    }

    /* Delete from DB before Insert*/
    if (!DBHelper::DeleteFromTable("DensityDetail", "task_name", task_info_["task_name"])) {
        return;
    }

    /* Insert To DB before Start RD*/
    if (!DBHelper::InsertToDensityDetail(kPersFile, kROIFile, kLMFile, task_info_["task_name"])) {
        return;
    }

    if (task_info_["task_type"] == kTaskTypeCounting) {
        /* Open RD Daemon For Train LM */
        StartRD();
    }
}

//void Delegate::ConnectStatus(bool flag)
//{
//    cv::Mat  img = cv::Mat::zeros(frame_height_, frame_width_, CV_8UC3);
//    if (flag) {
//        cv::putText(img, "Connecting ...", cv::Point(frame_width_/2 - 100, frame_height_/2), cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 2, cv::Scalar(255, 0, 0), 5, 16);
//    } else {
//        cv::putText(img, "Connect Error", cv::Point(frame_width_/2 - 100, frame_height_/2), cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 2, cv::Scalar(255, 0, 0), 5, 16);
//    }
//    p_src_clip_->ShowImage(img);
//    p_dst_clip_1_->ShowImage(img);
//    p_dst_clip_2_->ShowImage(img);
//    p_plot_->clear_plot();
//}

void Delegate::ConvertToSlice(const cv::Mat &src, cv::Mat &dst, cv::Point pt1, cv::Point pt2)
{
    //mat.at<cv::Vec3b>(y, x);
    /*Extract all the pixels on the raster line segment connecting two specified points. */
    vector<cv::Vec3b> line;
    ExtractLine(src, pt1, pt2, line);
    cv::Mat img = dst.clone();
    img(cv::Rect(1, 0, img.cols-1, img.rows)).copyTo(dst(cv::Rect(0, 0, dst.cols-1, dst.rows)));
    for(int i=0; i<line.size(); ++i) {
        cv::Vec3b val = line[i];
        dst.at<cv::Vec3b>(i, dst.cols-1)[0] = val[0];
        dst.at<cv::Vec3b>(i, dst.cols-1)[1] = val[1];
        dst.at<cv::Vec3b>(i, dst.cols-1)[2] = val[2];
    }
}

void Delegate::ExtractLine(const cv::Mat &src, cv::Point pt1, cv::Point pt2, vector<cv::Vec3b> &line)
{
    // grabs pixels along the line (pt1, pt2)
    // from 8-bit 3-channel image to the buffer
    /*iterate over all the pixels on the raster line segment connecting two specified points.*/
    cv::LineIterator it(src, pt1, pt2, 8);
    for(int i = 0; i < it.count; i++, ++it) {
        line.push_back((cv::Vec3b)*it);
    }
}

void Delegate::OrthogonalLine(const cv::Point &src_pt1, const cv::Point &src_pt2, const cv::Point &dst_pt1, cv::Point &dst_pt2, int dst_line_length)
{
    float angle, dx, dy;
    if (src_pt1.x == src_pt2.x) {
        dx = dst_pt1.x + dst_line_length;
        dy = dst_pt1.y;
    } else {
        angle = atan(-1 / (float(src_pt2.y - src_pt1.y) / float(src_pt2.x - src_pt1.x)));
        dx = dst_pt1.x + abs(dst_line_length*cos(angle));
        dy = dst_pt1.y + abs(dst_line_length*sin(angle));
    }

    dst_pt2 = cv::Point(dx, dy);
}

void Delegate::ArrowedLine(cv::Mat &img, cv::Point pt1, cv::Point pt2, const cv::Scalar &color, int thickness, int line_type, int shift, double tipLength)
{
    const double tipSize = norm(pt1-pt2)*tipLength; // Factor to normalize the size of the tip depending on the length of the arrow

    cv::line(img, pt1, pt2, color, thickness, line_type, shift);

    const double angle = atan2( (double) pt1.y - pt2.y, (double) pt1.x - pt2.x );

    cv::Point p(cvRound(pt2.x + tipSize * cos(angle + CV_PI / 4)),
        cvRound(pt2.y + tipSize * sin(angle + CV_PI / 4)));
    cv::line(img, p, pt2, color, thickness, line_type, shift);

    p.x = cvRound(pt2.x + tipSize * cos(angle - CV_PI / 4));
    p.y = cvRound(pt2.y + tipSize * sin(angle - CV_PI / 4));
    cv::line(img, p, pt2, color, thickness, line_type, shift);
}

void Delegate::release_memory()
{
    delete p_src_clip_;
    delete p_dst_clip_1_;
    delete p_dst_clip_2_;
    delete p_plot_;
    delete p_clip_alarm_;
}

