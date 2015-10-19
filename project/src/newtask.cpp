#include "newtask.h"
#include "utaskname.h"
#include "utasktype.h"
#include "utaskcamera.h"
#include "utaskalarm.h"
#include "utaskreport.h"
#include <QListWidget>
#include <QStackedWidget>
#include <QLayout>
#include <QPushButton>
#include "common.h"
#include "cameramanager.h"
#include "utility.h"
#include <QMessageBox>
#include "dbi.hpp"

NewTask::NewTask(QWidget *parent)
    : QDialog(parent)
{
}

NewTask::~NewTask()
{

}

void NewTask::Init(QString group_name)
{
//! [Init Parameter]
    group_name_ = group_name.toStdString();
    task_status_ = kStatusOFF;
    camera_status_ = kStatusOFF;
    report_switch_ = kStatusOFF;

//! [window setting]
    setWindowTitle("New Task");
    setAttribute(Qt::WA_DeleteOnClose);
    Utility::MoveToCenter(this);

//! [widget_main]
    QWidget *widget_main = new QWidget(this);
    QHBoxLayout *lay_main = new QHBoxLayout(widget_main);

//! [task_list]
    task_list_ = new QListWidget(widget_main);
    task_list_->setFont(QFont("Times", 12, QFont::Bold));
    task_list_->insertItem(0, tr("Name"));
    task_list_->insertItem(1, tr("Type"));
    task_list_->insertItem(2, tr("Source"));
    task_list_->insertItem(3, tr("Alarm"));
    task_list_->setDisabled(true);

//! [stackedwidget]
    QStackedWidget *stackedwidget = new QStackedWidget(widget_main);
    ui_taskname_ = new UTaskName(stackedwidget);
    ui_tasktype_ = new UTaskType(stackedwidget);
    ui_taskcamera_ = new UTaskCamera(stackedwidget);
    ui_taskalarm_ = new UTaskAlarm(stackedwidget);
    stackedwidget->addWidget(ui_taskname_);
    stackedwidget->addWidget(ui_tasktype_);
    stackedwidget->addWidget(ui_taskcamera_);
    stackedwidget->addWidget(ui_taskalarm_);

    lay_main->setMargin(5);
    lay_main->setSpacing(5);
    lay_main->addWidget(task_list_);
    lay_main->addWidget(stackedwidget, 0, Qt::AlignHCenter);
    lay_main->setStretchFactor(task_list_, 1);
    lay_main->setStretchFactor(stackedwidget, 3);
    lay_main->setSizeConstraint(QLayout::SetFixedSize);

//! [widget_btn]
    QWidget *widget_btn = new QWidget(this);
    QBoxLayout *lay_btn = new QHBoxLayout(widget_btn);

    btn_back_ = new QPushButton(tr("<Back"), widget_btn);
    btn_back_->hide();
    btn_next_ = new QPushButton(tr("Next>"), widget_btn);
    QPushButton *btn_cancel = new QPushButton(tr("Cancel"), widget_btn);

    lay_btn->addStretch();
    lay_btn->addWidget(btn_back_);
    lay_btn->addWidget(btn_next_);
    lay_btn->addWidget(btn_cancel);

//! [Layout]
    QVBoxLayout *layout = new QVBoxLayout;

    layout->addWidget(widget_main);
    layout->addWidget(widget_btn);

    setLayout(layout);

//! [setup connection]
    connect(task_list_, SIGNAL(currentRowChanged(int)), stackedwidget, SLOT(setCurrentIndex(int)));
    connect(btn_back_, SIGNAL(clicked()), this, SLOT(on_btnBack_clicked()));
    connect(btn_next_, SIGNAL(clicked()), this, SLOT(on_btnNext_clicked()));
    connect(btn_cancel, SIGNAL(clicked()), this, SLOT(deleteLater()));

    task_list_->setCurrentRow(0);

//! [Camera_manager]
    camera_manager_ = new CameraManager(0);
    connect(camera_manager_, &CameraManager::SelectedCamera, [=](QString str){
        ui_taskcamera_->set_camera(str);
        camera_manager_->deleteLater();
    });

    connect(ui_taskcamera_->btn_camera_, &QPushButton::clicked, [=](){
        camera_manager_->show();
    });
}

void NewTask::on_btnBack_clicked()
{
    switch (task_list_->currentRow()) {
    case 0:  // task name
        break;
    case 1:  // task type
        task_list_->setCurrentRow(0);
        btn_back_->hide();
        break;
    case 2:  // task camera
        task_list_->setCurrentRow(1);
        break;
    case 3:  // task alarm
        task_list_->setCurrentRow(2);
        btn_next_->setText("Next>");
        break;
    }
}

void NewTask::on_btnNext_clicked()
{
    switch (task_list_->currentRow()) {
    case 0:  // task name
        if (!set_task_name())
            return;
        task_path_ = kTaskRootPath + task_name_ + "/";
        task_list_->setCurrentRow(1);
        btn_back_->show();
        break;
    case 1:  // task type
        if (!set_task_type())
            return;
        task_list_->setCurrentRow(2);
        break;
    case 2: // task camera
        if (!set_task_source())
            return;
        task_list_->setCurrentRow(3);
        btn_next_->setText("Finish");
        break;
    case 3: // task alarm
        if (!set_alarm_strategy())
            return;

        // Insert To DB When Clicked Finish Button;
        if (!InsertNewTaskToDB()) {
            QMessageBox::critical(NULL, "critical", "DB Error!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            return;
        }

        /* Create Folder For ROI, Pers, LM, Alarm and GT
         * Delete Task From DB When Create Folder Failed */
        if (!CreateFiles()) {
            DeleteTask();
            return;
        }

        emit InsertTaskFinished();

        break;
    }
}

/* set task_name*/
bool NewTask::set_task_name()
{
    if (ui_taskname_->taskname().isEmpty()) {
        ui_taskname_->set_warning(QString("Task Name can't be Empty!"));
        return false;
    }
    if (IsTaskNameExist(ui_taskname_->taskname())) {
        ui_taskname_->set_warning(QString("Task Name already Exist!"));
        return false;
    }

    ui_taskname_->set_warning("");

    task_name_ = ui_taskname_->taskname().toStdString();

    return true;
}

/* set task_type */
bool NewTask::set_task_type()
{
    task_type_ = ui_tasktype_->tasktype().toStdString();

    return true;
}

/* set task_source */
bool NewTask::set_task_source()
{
    if (ui_taskcamera_->address().isEmpty()) {
        ui_taskcamera_->set_warning(QString("address can't be Empty!"));
        return false;
    }
    if (ui_taskcamera_->camera_type() == "Video") {
        if (!Utility::IsCameraValid(ui_taskcamera_->address().toStdString(), width_, height_, fps_, total_frames_)) {
            ui_taskcamera_->set_warning(ui_taskcamera_->camera_type() + QString(" Can't Open!"));
            return false;
        }
        // get video info
        camera_type_ = kCameraTypeFile;
        file_url_ = ui_taskcamera_->address().toStdString();

    } else {
        // get camera info
        camera_type_ = kCameraTypeHttp;
        camera_name_ = ui_taskcamera_->address().toStdString();
    }

    ui_taskcamera_->set_warning("");

    return true;
}

/* set alarm strategy */
bool NewTask::set_alarm_strategy()
{
    if (ui_taskalarm_->alarm_switch()) {
        if ((ui_taskalarm_->priority_low() == -1) || (ui_taskalarm_->priority_medium() == -1) || (ui_taskalarm_->priority_high() == -1)) {
            ui_taskalarm_->set_warning(QString("priority can't be empty!"));
            return false;
        }
        if ((ui_taskalarm_->priority_low() >= ui_taskalarm_->priority_medium()) || ui_taskalarm_->priority_medium() >= ui_taskalarm_->priority_high()) {
            ui_taskalarm_->set_warning(QString("priority must satisfy: low < medium < high !"));
            return false;
        }

        alarm_switch_ = kStatusON;
        priority_low_ = ui_taskalarm_->priority_low();
        priority_medium_ = ui_taskalarm_->priority_medium();
        priority_high_ = ui_taskalarm_->priority_high();

    } else {
        alarm_switch_ = kStatusOFF;
    }

    ui_taskalarm_->set_warning("");

    return true;
}

/* Create Files */
bool NewTask::CreateFiles()
{
    if (!Utility::CreateFileFolder(task_path_ + kPersDir))
        return false;
    if (!Utility::CreateFileFolder(task_path_ + kROIDir))
        return false;
    if (!Utility::CreateFileFolder(task_path_ + kLMDir))
        return false;
    if (!Utility::CreateFileFolder(task_path_ + kAlarmDir))
        return false;
    if (!Utility::CreateFileFolder(task_path_ + kGTDir))
        return false;

    return true;
}

bool NewTask::IsTaskNameExist(QString str)
{
    // Connect DB
    CDbi db;
    if (! Utility::IsDBConnected(db)) {
        QMessageBox::warning(0, "Warning", "Connect DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }

    // Check task_name in DB
    std::vector<std::map<std::string, std::string> > res = db.Query("SELECT task_name FROM Tasks;");

    for (size_t i = 0; i < res.size(); ++i) {
            if (str.toStdString() == res[i]["task_name"]) {
                return true;
            }
    }

    return false;
}

bool NewTask::InsertNewTaskToDB()
{
    if (!InsertToTasks())
        return false;

    if (camera_type_ == kCameraTypeFile) {
        if (!InsertToFiles()) {
            DeleteTask();
            return false;
        }
    } else {
        if (!InsertToTaskCamera()) {
            DeleteTask();
            return false;
        }
    }

    if (alarm_switch_ == kStatusON) {
        if (!InsertToDensityAlarmStrategy()) {
            DeleteTask();
            return false;
        }
    }

    return true;
}

bool NewTask::InsertToTasks()
{
    // Connect DB
    CDbi db;
    if (! Utility::IsDBConnected(db)) {
        QMessageBox::warning(0, "Warning", "Connect DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }
    // Insert to Tasks
    bool ok = db.RunSQL("INSERT INTO Tasks VALUES ('"
                         + task_name_ + "', '"
                         + task_type_ + "', '"
                         + task_status_ + "', '"
                         + camera_type_ + "', '"
                         + camera_status_ + "', '"
                         + task_path_ + "', '"
                         + alarm_switch_ + "', '"
                         + report_switch_ + "', '"
                         + group_name_ + "');");

    return ok;
}

bool NewTask::InsertToTaskCamera()
{
    // Connect DB
    CDbi db;
    if (! Utility::IsDBConnected(db)) {
        QMessageBox::warning(0, "Warning", "Connect DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }
    // Insert to Task_Camera
    bool ok = db.RunSQL("INSERT INTO Task_Camera VALUES ('"
                         + task_name_ + "', '"
                         + camera_name_ + "');");
    return ok;
}

bool NewTask::InsertToFiles()
{
    // Connect DB
    CDbi db;
    if (! Utility::IsDBConnected(db)) {
        QMessageBox::warning(0, "Warning", "Connect DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }
    // Insert to Files
    bool ok = db.RunSQL("INSERT INTO Files VALUE ('"
                         + file_url_ + "', "
                         + std::to_string(width_) + ", "
                         + std::to_string(height_) + ", "
                         + std::to_string(fps_) + ", "
                         + std::to_string(total_frames_) + ", '"
                         + task_name_ + "');");
    return ok;
}

bool NewTask::InsertToDensityAlarmStrategy()
{
    // Connect DB
    CDbi db;
    if (! Utility::IsDBConnected(db)) {
        QMessageBox::warning(0, "Warning", "Connect DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }
    // Insert to DensityAlarmStrategy
    bool ok = db.RunSQL("INSERT INTO DensityAlarmStrategy VALUES ("
                         + std::to_string(priority_low_) + ", "
                         + std::to_string(priority_medium_) + ", "
                         + std::to_string(priority_high_) + ", '"
                         + task_name_ + "');");
    return ok;
}

void NewTask::DeleteTask()
{
    // Connect DB
    CDbi db;
    if (! Utility::IsDBConnected(db)) {
        QMessageBox::warning(0, "Warning", "Connect DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    db.RunSQL("DELETE FROM Tasks WHERE task_name='" + task_name_ + "';");
}
