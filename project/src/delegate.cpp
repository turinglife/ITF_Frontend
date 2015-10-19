#include "delegate.h"
#include "uclipwidget.h"
#include "utility.h"
#include "ucountingsetting.h"
#include <QDebug>
#include <QSizePolicy>
#include <uplotwidget.h>
#include "alarmwidget.h"

Delegate::Delegate(QWidget *parent)
    : QWidget(parent)
    , curr_channel_(0)
    , curr_count_(0)
{
}

Delegate::~Delegate()
{
//    delete src_clip_;
//    delete dst_clip_;
}

void Delegate::Init(std::vector<std::map<std::string, std::string> > task_info, int channel)
{
    //! [Init Parameters]
    task_info_ = task_info[0];
    curr_channel_ = channel;
    camera_started_ = false;
    task_started_ = false;

    // Take care of Wrong Case
    if (task_info_["camera_status"] == kStatusOFF && task_info_["task_status"] == kStatusON) {
        task_info_["task_status"] = kStatusOFF;
        UpdateStatusToTasks("task_status", task_info_["task_status"]);
    }

    //! [Create new thread for execute command]
    command_center_ = new TaskCommandCenter(task_info_["task_name"], 0);
    command_center_->moveToThread(&thread_);

    //! [Create and Setup clipwidget for src and dst]
    src_clip_ = new UClipWidget(0);
    dst_clip_ = new UClipWidget(0);
    src_clip_->setVisible(false);
    dst_clip_->setVisible(false);
    src_clip_->set_curr_type(task_info_["task_type"]);
    Utility::SetLabelStyle(src_clip_);
    Utility::SetLabelStyle(dst_clip_);

//! [Create plot for Counting Tasks]
    if (task_info_["task_type"] == kTaskTypeCounting) {
        plot_ = new UPlotWidget(this);
        plot_->clear_plot();

        QSizePolicy plotSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        plotSizePolicy.setHorizontalStretch(0);
        plotSizePolicy.setVerticalStretch(0);
        plotSizePolicy.setHeightForWidth(plot_->sizePolicy().hasHeightForWidth());
        plot_->setSizePolicy(plotSizePolicy);
        plot_->setFocusPolicy(Qt::StrongFocus);
        plot_->setVisible(false);

//        if (camera_type == kCameraTypeFile) {
//            plot_->set_plot(0, 1000, camera_type);
//        } else {
//            plot_->set_plot(0, 180, camera_type);
//        }

        // currently view video as camera
        double curr_datetime = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
        plot_->set_plot(curr_datetime, curr_datetime+90.0, kCameraTypeHttp);

//! [Alarm]
        alarm_ = new AlarmWidget(this);
        std::vector<std::map<std::string, std::string> > res_alarm;
        if (!SelectFromTable("DensityAlarmStrategy", "task_name", task_info_["task_name"], res_alarm)) {
            return;
        }
        if (res_alarm.empty()) {
            std::string priority = "0";
            alarm_->set_alarm_strategy(false, priority, priority, priority);
        } else {
            alarm_->set_alarm_strategy(true, res_alarm[0]["priority_low"], res_alarm[0]["priority_medium"], res_alarm[0]["priority_high"]);
        }
    }

    //! [Setup Connecton]
    connect(command_center_, SIGNAL(CommandExecuted(QString,QString,bool)), this, SLOT(MessageReceiver(QString,QString,bool)));
    connect(this, SIGNAL(CameraStoped()), command_center_, SLOT(deleteLater()));
    connect(this, SIGNAL(CameraStoped()), &thread_, SLOT(quit()));
    connect(&thread_, SIGNAL(started()), this, SLOT(StartCD()));
    connect(&thread_, &QThread::finished, [=](){
        emit RemoveFromChannel(curr_channel_);
    });
    // Setup btn
    connect(this, &Delegate::CameraStarted, [=](){
        // Start Timer
        timer.start(1000/20);

        src_clip_->set_start_enabled(true);
        src_clip_->set_stop_enabled(false);
        src_clip_->set_clear_enabled(true);

        /* Check task_status
         * IF CD Start Success, Then Start AD */
        if (task_info_["task_status"] == kStatusON) {
            StartAD();
        }
    });
    connect(this, &Delegate::TaskStarted, [=](){
        src_clip_->set_start_enabled(false);
        src_clip_->set_stop_enabled(true);
        src_clip_->set_countingsetting_enabled(false);
    });
    connect(this, &Delegate::TaskStoped, [=](){
        src_clip_->set_start_enabled(true);
        src_clip_->set_stop_enabled(false);
        src_clip_->set_countingsetting_enabled(true);
    });

    connect(src_clip_, SIGNAL(Command(QString,QString)), this, SLOT(CommandAssigner(QString,QString)));
    connect(src_clip_, SIGNAL(RemoveTaskFromBoard()), this, SLOT(StopCD()));
    connect(src_clip_, SIGNAL(CountingSetting()), this, SLOT(onCountingSettingClicked()));
    connect(src_clip_, &UClipWidget::ShowAnalyzeBoard, [=](){
        if (camera_started_ && task_started_) {
            emit ShowAnalyzeBoard(curr_channel_);
        }
    });

    //! [Start Thread]
    thread_.start();

    //! [Start timer]
    connect(&timer, SIGNAL(timeout()), this, SLOT(PlayCamera()));
}

string Delegate::task_name()
{
    return task_info_["task_name"];
}

string Delegate::task_type()
{
    return task_info_["task_type"];
}

UClipWidget *Delegate::src_clip()
{
    return src_clip_;
}

UClipWidget *Delegate::dst_clip() const
{
    return dst_clip_;
}

UPlotWidget *Delegate::plot() const
{
    return plot_;
}

AlarmWidget *Delegate::alarm()
{
    return alarm_;
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
            return;
        }
        cv::Mat img_src = curr_src_.clone();

        //! [Get Analyze Results from Buffer]
        if (task_started_) {
            // If fetch dst success, then update dst_map, plot and alarm;
            if (buffer_.fetch_dst(curr_dst_, curr_count_)) {
                if (task_info_["task_type"] == kTaskTypeCounting) {
                    // counting task, Plot, view video as camera
                    double key;
                    if (task_info_["camera_type"] == kCameraTypeFile) {
                        key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
                    } else {
                        key = static_cast<double> (timestamp_);
                    }

                    plot_->set_data(key, curr_count_);

                    // Alarm
                    alarm_->set_value(curr_count_);
                }

                // dst map
                dst_clip_->setPixmap(QPixmap::fromImage(QImage(curr_dst_.data, curr_dst_.cols, curr_dst_.rows, curr_dst_.step, QImage::Format_RGB888).rgbSwapped()));
            }

            if (task_info_["task_type"] == kTaskTypeCounting) {
                // Counting Task, Always Show ROI and Counting result, no matter fetch dst succuss or not
                for (int i=0; i<roi_points_.size()-1; ++i) {
                    cv::line(img_src, roi_points_[i], roi_points_[i+1], cv::Scalar( 0, 0, 255),  2, 8 );
                }
                cv::line(img_src, roi_points_.first(), roi_points_.last(), cv::Scalar( 0, 0, 255),  2, 8 );
                cv::putText(img_src, std::to_string(curr_count_), cv::Point(20, 60), cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 2, cv::Scalar(0, 0, 255), 2, 8);
            }
        }

        src_clip_->setPixmap(QPixmap::fromImage(QImage(img_src.data, img_src.cols, img_src.rows, img_src.step, QImage::Format_RGB888).rgbSwapped()));
    }
}

void Delegate::StartCD()
{
    CommandAssigner(QString::fromStdString(kDaemonCD), QString::fromStdString(kCommandStart));
}

void Delegate::StartAD()
{
    CommandAssigner(QString::fromStdString(kDaemonAD), QString::fromStdString(kCommandStart));
}

void Delegate::StartRD()
{
    CommandAssigner(QString::fromStdString(kDaemonRD), QString::fromStdString(kCommandStart));
}

void Delegate::StopCD()
{
    CommandAssigner(QString::fromStdString(kDaemonCD), QString::fromStdString(kCommandStop));
}

void Delegate::StopAD()
{
    CommandAssigner(QString::fromStdString(kDaemonAD), QString::fromStdString(kCommandStop));
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
    }

    /* Stop CD
     * -- Stop AD
     * -- Set camera_started_ to false so that CD can destroy ShareMemory */
     if (daemon == QString::fromStdString(kDaemonCD) && operation == QString::fromStdString(kCommandStop)) {
        if (!camera_started_) {
            // CD already stopped, do nothing
            return;
        }
         if (task_started_) {
             // If AD is ON, then need to Stop it and manually retry Stop CD
             QMessageBox::warning(NULL, "warning", "Please Stop AD First!", QMessageBox::Ok, QMessageBox::Ok);
             return;
         }
         // Release ShareMemory in CD Require Stop play camera in the Front_End
         // Note: If Stop CD FAILDE in the end, we need to recover below status
         camera_started_ = false;
     }

    /* Start AD,
     * Need to Check (if task type is counting):
     *  -- ROI, PersMap, lm */
     if (daemon == QString::fromStdString(kDaemonAD) && operation == QString::fromStdString(kCommandStart)) {
         if (task_started_) {
             // AD already started, do nothing
             return;
         }
         if (task_info_["task_type"] == kTaskTypeCounting) {
             if (!IsReadyToStartAD()) {
                 QMessageBox::StandardButton btn_message = QMessageBox::warning(NULL, "warning", "Please Setup Task! \n Setup Right Now?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
                 if (btn_message == QMessageBox::Yes) {
                     onCountingSettingClicked();
                 }
                 return;
             }
         }
     }

    /* Stop AD
     * Need to:
     * -- Set task_started_ to false */
     if (daemon == QString::fromStdString(kDaemonAD) && operation == QString::fromStdString(kCommandStop)) {
         if (!task_started_) {
             // AD already stopped, do nothing
             return;
         }
         // Note: If Stop AD FAILDE in the end, we need to recover below status
         task_started_ = false;
     }

     /* Processing Command in another thread */
    if (operation == QString::fromStdString(kCommandStart)) {
        QMetaObject::invokeMethod(command_center_, "OpenDaemon", Qt::QueuedConnection, Q_ARG(QString, daemon), Q_ARG(QString, operation));
    } else {
        QMetaObject::invokeMethod(command_center_, "ExecuteCommand", Qt::QueuedConnection, Q_ARG(QString, daemon), Q_ARG(QString, operation));
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
                /* CD START SUCCESS
                 * -- Init Buffer
                 * -- Set camera_started_ to true
                 * -- Update camera_status==ON to DB */
                if (!InitBuffer()) {
                    qDebug() << "Init Buffer Fail";
                    emit CameraStoped();
                    return;
                }

                camera_started_ = true;
                task_info_["camera_status"] = kStatusON;
                if (!UpdateStatusToTasks("camera_status", kStatusON)) {
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
            if (flag) {
                /* CD STOP SUCCESS
                 * -- Update camera_status == OFF to DB
                 * -- Exit CommandCenter Thread -- Set*/
                task_info_["camera_status"] = kStatusOFF;
                if (!UpdateStatusToTasks("camera_status", kStatusOFF)) {
                    return;
                }

                emit CameraStoped();
            } else {
                /* CD STOP FAILED
                 * Recover the status, which was changed in CommandAssigner Stop CD part
                 * -- Set camera_started_ to true, Since Stop CD FAILED */
                camera_started_ = true;

                QMessageBox::warning(NULL, "warning", "CD STOP Failed! ", QMessageBox::Ok, QMessageBox::Ok);
            }
        }
    }

    /* AD */
    if (daemon == QString::fromStdString(kDaemonAD)) {
        /* START */
        if (operation == QString::fromStdString(kCommandStart)) {
            if (flag) {
                /* AD START SUCCESS
                 * -- Set task_started_ to true
                 * -- Update task_status==ON to DB */
                task_started_ = true;

                task_info_["task_status"] = kStatusON;
                if (!UpdateStatusToTasks("task_status", kStatusON)) {
                    return;
                }

                emit TaskStarted();
            } else {
                /* AD START FAILED */
                QMessageBox::warning(NULL, "warning", "AD START Failed! ", QMessageBox::Ok, QMessageBox::Ok);
            }
        }
        /* STOP */
        if (operation == QString::fromStdString(kCommandStop)){
            if (flag) {
                /* AD STOP SUCCESS */
                task_info_["task_status"] = kStatusOFF;
                if (!UpdateStatusToTasks("task_status", kStatusOFF)) {
                    return;
                }

                emit TaskStoped();
            } else {
                /* AD STOP FAILED
                 * Recover the status, which was changed in CommandAssigner Stop AD part
                 * -- Set task_started_ to true, Since Stop CD FAILED */
                task_started_ = true;

                QMessageBox::warning(NULL, "warning", "AD STOP Failed! ", QMessageBox::Ok, QMessageBox::Ok);
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
}

void Delegate::onCountingSettingClicked()
{
    countingsetting_ = new UCountingSetting(0);
    Utility::MoveToCenter(countingsetting_);
    countingsetting_->show();

    ReadCountingSetting();

    connect(countingsetting_, &UCountingSetting::BtnSnapshotClicked, [=](){
        countingsetting_->Snapshot(curr_src_);
    });

    connect(countingsetting_, &UCountingSetting::BtnFinishClicked, [=](){
        WriteCountingSetting();
        countingsetting_->deleteLater();
    });
}

bool Delegate::InitBuffer()
{
    // Init Buffer
    if (!buffer_.Init(task_info_["task_name"])) {
        qDebug() << "Cannot Init Buffer";
        return false;
    }
    // get frame size
    buffer_.frame_size(frame_width_, frame_height_);

    // Init curr_src_ and curr_dst_
    curr_src_ = cv::Mat::zeros(frame_height_, frame_width_, CV_8UC3);
    curr_dst_ = cv::Mat::zeros(frame_height_, frame_width_, CV_8UC3);

    // Init frame_sketchpad_rate
    frame_sketchpad_rate_w_ = float(frame_width_) / float(kSketchpadWidth);
    frame_sketchpad_rate_h_ = float(frame_height_) / float(kSketchpadHeight);

    return true;
}

bool Delegate::IsReadyToStartAD()
{
    std::vector<std::map<std::string, std::string> > res;
    if (!SelectFromTable("DensityDetail", "task_name", task_info_["task_name"], res)) {
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
    if (!Utility::LoadPoints(roi_points_, roi_path, 1, 1)) {
        return false;
    }

    return true;
}

void Delegate::ReadCountingSetting()
{
    // Select From DB
    std::vector<std::map<std::string, std::string> > res;
    if (!SelectFromTable("DensityDetail", "task_name", task_info_["task_name"], res)) {
        return;
    }
    if (res.empty()) {
        return;
    }

    std::string roi_path = task_info_["task_path"] + kROIDir + res[0]["roi_file"];
    std::string pers_path= task_info_["task_path"] + kPersDir + res[0]["pers_file"];
    std::string gt_dir = task_info_["task_path"] + kGTDir;

    // Load ROI Points for Sketchpad
    QVector<QPointF> roi_points;
    if (!Utility::LoadPoints(roi_points, roi_path, 1/frame_sketchpad_rate_w_, 1/frame_sketchpad_rate_h_)) {
        return;
    }
    countingsetting_->set_roi_points(curr_src_, roi_points);

    // Load Pers Points for Sketchpad
    QVector<QPointF> pers_points;
    if (!Utility::LoadPoints(pers_points, pers_path,1/ frame_sketchpad_rate_w_, 1/frame_sketchpad_rate_h_)) {
        return;
    }
    countingsetting_->set_pers_points(curr_src_, pers_points);

    /* get img and label data from gt_dir */
    if (!boost::filesystem::is_directory(gt_dir)) {
        cout << gt_dir << " don't exist" << endl;
        return;
    }
    QVector<QVector<QPointF> > gt_points;
    QVector<cv::Mat> gt_images;
    boost::filesystem::directory_iterator end_iter;
    for (boost::filesystem::directory_iterator file_iter(gt_dir); file_iter != end_iter; ++file_iter) {

        if (!boost::filesystem::is_directory(*file_iter) && (boost::filesystem::extension(*file_iter) == kGTImageSuffix)) {
            boost::filesystem::path p = file_iter->path();
            std::string gt_img_file = p.string();
            std::string gt_point_file = p.replace_extension(kGTAnnotationSuffix).string();

            // Load GT Points for Sketchpad
            QVector<QPointF> gt_point;
            if (!Utility::LoadPoints(gt_point, gt_point_file, 1/frame_sketchpad_rate_w_, 1/frame_sketchpad_rate_h_)) {
                return;
            }
            gt_points.append(gt_point);

            // Load Image
            cv::Mat img = cv::imread(gt_img_file);
            gt_images.append(img);
        }
    }
    countingsetting_->set_gt_points(gt_images, gt_points);
}

void Delegate::WriteCountingSetting()
{
    // get roi_path, pers_path and gt_dir
    std::string roi_path = task_info_["task_path"] + kROIDir + kROIFile;
    std::string pers_path = task_info_["task_path"] + kPersDir + kPersFile;
    std::string gt_dir = task_info_["task_path"] + kGTDir;

    /* save roi_file and pers_file from Sketchpad*/
    if (!Utility::SavePoints(countingsetting_->roi_points(), roi_path, frame_sketchpad_rate_w_, frame_sketchpad_rate_h_)) {
        return;
    }
    if (!Utility::SavePoints(countingsetting_->pers_points(), pers_path, frame_sketchpad_rate_w_, frame_sketchpad_rate_h_)) {
        return;
    }

    /* Remove GT First */
    if (!Utility::RemoveFileFolder(gt_dir)) {
        return;
    }
    /* Create GT Folder*/
    if (!Utility::CreateFileFolder(gt_dir)) {
        return;
    }

    for (int i=0; i<countingsetting_->gt_images().size(); ++i) {
        std::string gt_image_path = gt_dir + std::to_string(i) + kGTImageSuffix;
        std::string gt_annotation_path = gt_dir + std::to_string(i) + kGTAnnotationSuffix;
        /* save gt_points*/
        if (!Utility::SavePoints(countingsetting_->gt_points()[i], gt_annotation_path, frame_sketchpad_rate_w_, frame_sketchpad_rate_h_)) {
            return;
        }
        /* save images */
        cv::imwrite(gt_image_path, countingsetting_->gt_images()[i]);
    }

    /* Delete from DB before Insert*/
    DeleteFromTable("DensityDetail", "task_name", task_info_["task_name"]);

    /* Insert To DB before Start RD*/
    if (!InsertToDensityDetail()) {
        return;
    }

    /* Open RD Daemon For Train LM */
    StartRD();
}

bool Delegate::InsertToDensityDetail()
{
    // Connect to DB
    CDbi db;
    if (! Utility::IsDBConnected(db)) {
        QMessageBox::warning(0, "Warning", "Connect DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }

    bool ok = db.RunSQL("INSERT INTO DensityDetail VALUES ('"
                         + kPersFile+ "', '"
                         + kROIFile + "', '"
                         + kLMFile + "', '"
                         + task_info_["task_name"] + "');");
    return ok;
}

bool Delegate::SelectFromTable(const string &table, const string &key, const string &val, std::vector<std::map<string, string> > &res)
{
    // Connect to DB
    CDbi db;
    if (! Utility::IsDBConnected(db)) {
        QMessageBox::warning(0, "Warning", "Connect DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }

    // Get Info From Table
    res = db.Query("SELECT * FROM " + table + " WHERE " + key + "='" + val + "';");

    return true;
}

bool Delegate::UpdateStatusToTasks(const string &key, const string &status)
{
    // Connect to DB
    CDbi db;
    if (! Utility::IsDBConnected(db)) {
        QMessageBox::warning(0, "Warning", "Connect DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }

    bool ok = db.RunSQL("UPDATE Tasks SET " + key + "='" + status + "' WHERE task_name='" + task_info_["task_name"] + "';");
    if (!ok) {
        QMessageBox::warning(NULL, "warning", "Update DB Error!", QMessageBox::Ok, QMessageBox::Ok);
         return false;
    }

    return true;
}

void Delegate::DeleteFromTable(const string &table, const string &key, const string &val)
{
    // Connect to DB
    CDbi db;
    if (! Utility::IsDBConnected(db)) {
        QMessageBox::warning(0, "Warning", "Connect DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    db.RunSQL("DELETE FROM " + table + " WHERE " + key + "='" + val + "';");
}
