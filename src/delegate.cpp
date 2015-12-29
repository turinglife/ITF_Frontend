#include "delegate.h"
#include "utility.h"

Delegate::Delegate(QWidget *parent) : QWidget(parent)
{

}

Delegate::~Delegate()
{
    std::cout << "release Delegate begin" << std::endl;
    release_momery();
    std::cout << "release Delegate done" << std::endl;
}

void Delegate::Init(std::vector<std::map<string, string> > task_info, int channel)
{
//! [Init Parameters]
    task_info_ = task_info[0];
    curr_channel_ = channel;
    camera_started_ = false;
    task_started_ = false;
    alarm_started_ = false;
    video_finished_ = false;

    fps_ = 20;

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

//! [Create and Setup clipwidget for src and dst]
#ifdef USE_GLWIDGET
    p_src_clip_ = new UClipGLWidget(0);
    p_dst_clip_ = new UClipGLWidget(0);
    Utility::SetWdgSizePolicy(p_src_clip_, QSizePolicy::Ignored, QSizePolicy::Ignored);
    Utility::SetWdgSizePolicy(p_dst_clip_, QSizePolicy::Ignored, QSizePolicy::Ignored);
#else
    p_src_clip_ = new UClipWidget(this);
    p_dst_clip_ = new UClipWidget(this);
    Utility::SetLabelStyle(p_src_clip_);
    Utility::SetLabelStyle(p_dst_clip_);
#endif
    p_src_clip_->set_curr_name(task_info_["task_name"]);
    p_src_clip_->set_curr_type(task_info_["task_type"]);
    p_src_clip_->setVisible(false);
    p_dst_clip_->setVisible(false);

//! [Create plot for Counting Tasks]
    p_plot_ = new UPlotWidget(this);
    p_plot_->clear_plot();
    Utility::SetWdgSizePolicy(p_plot_, QSizePolicy::Expanding, QSizePolicy::Expanding);
    p_plot_->setVisible(false);
    // currently view video as camera
    double curr_datetime = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    p_plot_->set_plot(curr_datetime, curr_datetime+90.0, kCameraTypeHttp);

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

//! [Setup Connecton]
    connect(p_command_center_, SIGNAL(CommandExecuted(QString,QString,bool)), this, SLOT(MessageReceiver(QString,QString,bool)));
    connect(this, SIGNAL(CameraStoped()), p_command_center_, SLOT(deleteLater()));
    connect(this, SIGNAL(CameraStoped()), &thread_, SLOT(quit()));
    connect(this, SIGNAL(CameraStoped()), &timer_, SLOT(stop()));
    connect(&thread_, SIGNAL(started()), this, SLOT(StartCD()));
    connect(&thread_, SIGNAL(finished()), this, SLOT(OnThreadFInished()));
    connect(this, SIGNAL(CameraStarted()), this, SLOT(OnCameraStarted()));
    connect(this, SIGNAL(TaskStarted()), this, SLOT(OnTaskStarted()));
    connect(this, SIGNAL(TaskStoped()), this, SLOT(OnTaskStoped()));
    connect(p_src_clip_, SIGNAL(Command(QString,QString)), this, SLOT(CommandAssigner(QString,QString)));
    connect(p_src_clip_, SIGNAL(RemoveTaskFromBoard()), this, SLOT(StopCD()));
    connect(p_src_clip_, SIGNAL(Setting()), this, SLOT(OnCountingSettingClicked()));
    connect(&timer_, SIGNAL(timeout()), this, SLOT(PlayCamera()));
    connect(p_src_clip_, SIGNAL(ShowClipAnalysisBoard()), this, SLOT(OnShowClipAnalysisBoardClicked()));

//! [Start Thread]
    thread_.start();
}

#ifdef USE_GLWIDGET
UClipGLWidget *Delegate::src_clip()
{
    return p_src_clip_;
}

UClipGLWidget *Delegate::dst_clip() const
{
    return p_dst_clip_;
}
#else
UClipWidget *Delegate::src_clip()
{
    return p_src_clip_;
}

UClipWidget *Delegate::dst_clip() const
{
    return p_dst_clip_;
}
#endif

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

void Delegate::StartAD()
{
    CommandAssigner(QString::fromStdString(kDaemonAD), QString::fromStdString(kCommandStart));
}
void Delegate::StopCD()
{
    CommandAssigner(QString::fromStdString(kDaemonCD), QString::fromStdString(kCommandStop));
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



void Delegate::PlayCamera()
{
    /* ABOUT camera_started and task_started
     * Note:
     * Set those status equal to false is in CommandAssigner Stop(AD/CD) Part,
     * Since We need to stop display frames in Front_End before we Stop(AD/CD);
     * Set those status equal to true is in MessageReceiver Start(AD/CD) Part,
     * Since We need to Start(AD/CD) before we can display frames in Front_End; */
    if (camera_started_) {
        //! [Get Src from Buffer]
        if (!buffer_.fetch_src(curr_src_, timestamp_)) {
            if (!buffer_.is_camera_valid()) { /// Video is over
                VideoIsFinished();
            }
            return;
        }
        cv::cvtColor(curr_src_, curr_src_, CV_BGR2RGB);
        cv::Mat img_src = curr_src_.clone();

        //! [Get Analyze Results from Buffer]
        if (task_started_) {
            // If fetch dst success, then update dst_map, plot and alarm;
            cv::Mat img_src_discard = cv::Mat::zeros(frame_height_, frame_width_, CV_8UC3);
            unsigned int timestamp_discard;
            if (buffer_.fetch_dst(img_src_discard, curr_dst_, curr_count_, timestamp_discard)) {
                cv::cvtColor(curr_dst_, curr_dst_, CV_BGR2RGB);
                // counting task, Plot, view video as camera
                if (task_info_["task_type"] == kTaskTypeCounting) {
                    double key;
                    if (task_info_["camera_type"] == kCameraTypeFile)
                        key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
                    else
                        key = static_cast<double> (timestamp_);

                    p_plot_->set_data(key, curr_count_);

                    // Alarm
                    p_clip_alarm_->set_value(curr_count_);
                }
                // dst map
                p_dst_clip_->ShowImage(curr_dst_);
            }
            // Counting Task, Always Show ROI and Counting result, no matter fetch dst succuss or not
            if (task_info_["task_type"] == kTaskTypeCounting) {
                for (int i=0; i<roi_points_.size()-1; ++i)
                    cv::line(img_src, roi_points_[i], roi_points_[i+1], cv::Scalar( 0, 0, 255),  2, 8 );
                cv::line(img_src, roi_points_.first(), roi_points_.last(), cv::Scalar( 0, 0, 255),  2, 8 );
                cv::putText(img_src, std::to_string(curr_count_), cv::Point(20, 60), cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 2, cv::Scalar(0, 0, 255), 2, 8);
            }
        }

        p_src_clip_->ShowImage(img_src);
    }
}

void Delegate::CommandAssigner(QString daemon, QString operation)
{
    qDebug() << daemon << " " << operation;
    /* Start CD */
    if (daemon == QString::fromStdString(kDaemonCD) && operation == QString::fromStdString(kCommandStart)) {
        if (camera_started_) {
            // CD alreay started, do nothing.
            return;
        }

        p_src_clip_->set_clip_controler(false, false, false, false, false);
    }

    /* Stop CD */
     if (daemon == QString::fromStdString(kDaemonCD) && operation == QString::fromStdString(kCommandStop)) {
        if (!camera_started_ || task_started_) {
            // CD already stopped or AD still runing do nothing
             return;
         }
        p_src_clip_->set_clip_controler(false, false, false, false, false);

        task_info_["camera_status"] = kStatusOFF;
        if (! DBHelper::UpdateStatusToTasks("camera_status", task_info_["camera_status"], task_info_["task_name"])) {
            QMessageBox::warning(0, "Warning", "Update DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
            return;
        }

         // Release ShareMemory in CD Require Stop play camera in the Front_End
         timer_.stop();
         camera_started_ = false;
     }

    /* Start AD,
     * Need to Check (if task type is counting): -- ROI, PersMap, lm */
     if (daemon == QString::fromStdString(kDaemonAD) && operation == QString::fromStdString(kCommandStart)) {
         if (!camera_started_ || task_started_) {
             // CD stopped or AD already started do nothing.
             return;
         }

         p_src_clip_->set_clip_controler(false, false, false, false, false);
         if (task_info_["task_type"] == kTaskTypeCounting) {
             if (!IsReadyToStartAD()) {
                 p_src_clip_->set_clip_controler(true, true, false, false, true);
                 QMessageBox::information(NULL, "warning", "Please Setup Task!", QMessageBox::Ok, QMessageBox::Ok);
                 return;
             }
         }
     }

    /* Stop AD */
     if (daemon == QString::fromStdString(kDaemonAD) && operation == QString::fromStdString(kCommandStop)) {
         if (!task_started_) {
             // AD already stopped, do nothing
             return;
         }
         p_src_clip_->set_clip_controler(false, false, false, false, false);

         task_info_["task_status"] = kStatusOFF;
         if (!DBHelper::UpdateStatusToTasks("task_status", task_info_["task_status"], task_info_["task_name"])) {
             QMessageBox::warning(0, "Warning", "Update DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
             return;
         }

         /* Need to Set task_started_ to false */
         timer_.stop();
         task_started_ = false;
         timer_.start(1000/fps_);
     }

     /* Start MD */
      if (daemon == QString::fromStdString(kDaemonMD) && operation == QString::fromStdString(kCommandStart)) {
          if (alarm_started_)
              return;
      }

     /* Stop MD */
      if (daemon == QString::fromStdString(kDaemonMD) && operation == QString::fromStdString(kCommandStop)) {
          if (!alarm_started_)
              return;
          alarm_started_ = false;
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
                if (!InitBuffer()) {
                    emit CameraStoped();
                    return;
                }
                 // -- Set camera_started_ to true
                camera_started_ = true;
                timer_.start(1000/fps_);
                 //-- Update camera_status==ON to DB */
                task_info_["camera_status"] = kStatusON;
                if (!DBHelper::UpdateStatusToTasks("camera_status", task_info_["camera_status"], task_info_["task_name"])) {
                    QMessageBox::warning(0, "Warning", "Update DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
                    return;
                }

                emit CameraStarted();
            } else {
                // CD START FAILED
                emit CameraStoped();
                QMessageBox::warning(NULL, "warning", "CD START Failed! ", QMessageBox::Ok, QMessageBox::Ok);
            }
        }

        /*STOP*/
        if (operation == QString::fromStdString(kCommandStop)){
                emit CameraStoped();
            if (flag) {
                /* CD STOP SUCCESS */
            } else {
            }
        }
    }

    /* AD */
    if (daemon == QString::fromStdString(kDaemonAD)) {
        /* START */
        if (operation == QString::fromStdString(kCommandStart)) {
            if (flag) {
                /* AD START SUCCESS*/
                // -- Set task_started_ to true
                timer_.stop();
                task_started_ = true;
                timer_.start(1000/fps_);
                /* -- Update task_status==ON to DB */
                task_info_["task_status"] = kStatusON;
                if (! DBHelper::UpdateStatusToTasks("task_status", task_info_["task_status"], task_info_["task_name"])) {
                    QMessageBox::warning(0, "Warning", "Update DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
                    return;
                }

                emit TaskStarted();
            } else {
                /* AD START FAILED */
                emit TaskStoped();
                QMessageBox::warning(NULL, "warning", "AD START Failed! ", QMessageBox::Ok, QMessageBox::Ok);
            }
        }
        /* STOP */
        if (operation == QString::fromStdString(kCommandStop)){
                emit TaskStoped();
            if (flag) {
                /* AD STOP SUCCESS */
            } else {
            }
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
            } else {
                /* MD START FAILED */
            }
        }
        /* STOP */
        if (operation == QString::fromStdString(kCommandStop)){
            if (flag) {
                /* MD STOP SUCCESS */
            } else {
                /* MD STOP FAILED */
            }
        }
    }

}

void Delegate::OnCameraStarted()
{
    if (task_info_["task_status"] == kStatusOFF) {
        p_src_clip_->set_clip_controler(true, true, false, false, true);
    } else {
        // startAD when task_status is ON in DB
        StartAD();
    }
}

void Delegate::OnTaskStarted()
{
    p_src_clip_->set_clip_controler(false, false, true, true, false);
    if (task_info_["alarm_switch"] == kStatusON) {
        StartMD();
    }
}

void Delegate::OnTaskStoped()
{
    if (task_info_["alarm_switch"] == kStatusON) {
        StopMD();
    }
    if (video_finished_) {
        p_src_clip_->set_clip_controler(true, false, false, false, false);
    } else {
        p_src_clip_->set_clip_controler(true, true, false, false, true);
    }
}

void Delegate::OnThreadFInished()
{
    emit RemoveFromChannel(curr_channel_);
}

void Delegate::OnShowClipAnalysisBoardClicked()
{
    if (camera_started_ && task_started_) {
        emit ShowClipAnalysisBoard(curr_channel_);
    }
}

void Delegate::OnCountingSettingClicked()
{
//! [p_countingsetting]
    p_countingsetting_ = new CountingSetting(0);

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
        qDebug() << "Cannot Init Buffer";
        return false;
    }
    buffer_.frame_size(frame_width_, frame_height_);

    // Init curr_src_ and curr_dst_
    curr_src_ = cv::Mat::zeros(frame_height_, frame_width_, CV_8UC3);
    curr_dst_ = cv::Mat::zeros(frame_height_, frame_width_, CV_8UC3);

    return true;
}

bool Delegate::IsReadyToStartAD()
{
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

    return true;
}

bool Delegate::VideoIsFinished()
{
    video_finished_ = true;
    // When Video is Finished, stop AD, CD
    if (task_started_) {
        // stop AD
        StopAD();
    } else {
        p_src_clip_->set_clip_controler(true, false, false, false, false);
    }

    int index=0;
    while (++index < 5) {
        if (task_started_ || alarm_started_) {
            sleep(0.5);
        } else {
            break;
        }
    }

    timer_.stop();
    // If stop cd here, make sure you back to clipboard first if you currently in clip_analysis_board.    //StopCD();
//    cv::Mat img = curr_src_.clone();
    cv::Mat  img = cv::Mat::zeros(frame_height_, frame_width_, CV_8UC3);
    cv::putText(img, "Finished", cv::Point(frame_width_/2 - 100, frame_height_/2), cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 2, cv::Scalar(255, 0, 0), 5, 16);
    p_src_clip_->ShowImage(img);
    p_dst_clip_->ShowImage(img);
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
    std::string gt_dir = task_info_["task_path"] + kGTDir;

    QVector<QPointF> roi_points;
    QVector<QPointF> pers_points;
    QVector<QVector<QPointF> > gt_points;
    QVector<cv::Mat> gt_images;

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

    /* get img and label data from gt_dir */
    if (!Utility::ReadGTData(gt_dir, gt_points, gt_images)) {
        return false;
    }
    p_countingsetting_->set_gt_points_images(gt_images, gt_points);

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
    /*save gt data*/
    if (!Utility::WriteDGData(gt_dir, p_countingsetting_->gt_points(), p_countingsetting_->gt_images())) {
        return;
    }
    /* Delete from DB before Insert*/
    if (!DBHelper::DeleteFromTable("DensityDetail", "task_name", task_info_["task_name"])) {
        return;
    }

    /* Insert To DB before Start RD*/
    if (!DBHelper::InsertToDensityDetail(kPersFile, kROIFile, kLMFile, task_info_["task_name"])) {
        return;
    }

    /* Open RD Daemon For Train LM */
    StartRD();
}

void Delegate::release_momery()
{
    delete p_src_clip_;
    delete p_dst_clip_;
    delete p_plot_;
    delete p_clip_alarm_;
}

