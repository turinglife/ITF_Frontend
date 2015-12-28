#include "taskwidget.h"
#include "utility.h"
#include <utname.h>
#include <uttype.h>
#include <utcamera.h>
#include <utalarmcounting.h>
#include <QStackedWidget>
#include <QLayout>

TaskWidget::TaskWidget(QWidget *parent) : QWidget(parent)
{

}

TaskWidget::~TaskWidget()
{

}

void TaskWidget::Init(QString group_name)
{
    //! [window setting]
    setWindowTitle("New Task");
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);
    Utility::MoveToCenter(this);

//! [Init Parameter]
    group_name_ = group_name.toStdString();
    task_status_ = kStatusOFF;
    camera_status_ = kStatusOFF;
    report_switch_ = kStatusOFF;

    QVBoxLayout *p_layout = new QVBoxLayout;
//! [widget_main]
    QWidget *p_wgt_main = new QWidget(this);
    QHBoxLayout *p_lay_main = new QHBoxLayout(p_wgt_main);

//! [task_list]
    p_task_list_ = new QListWidget(p_wgt_main);
    p_task_list_->setFont(QFont("Times", 12, QFont::Bold));
    p_task_list_->insertItem(0, tr("Task Name"));
    p_task_list_->insertItem(1, tr("Task Type"));
    p_task_list_->insertItem(2, tr("Choose Source"));
    p_task_list_->insertItem(3, tr("Alarm Strategy"));
    p_task_list_->setDisabled(true);
    p_task_list_->setCurrentRow(0);

//! [stackedwidget]
    p_stackedwidget_ = new QStackedWidget(p_wgt_main);
    p_utname_ = new UTName(p_stackedwidget_);
    p_utname_->set_guide(QString("Please Input a Task Name."));
    p_uttype_ = new UTType(p_stackedwidget_);
    p_uttype_->set_guide(QString("Please Choose a Task Type."));
    p_utcamera_ = new UTCamera(p_stackedwidget_);
    p_utcamera_->set_guide(QString("Please Select a Video/Camera for this Task."));
    p_utalarmcounting_ = new UTAlarmCounting(p_stackedwidget_);
    p_utalarmcounting_->set_guide(QString("Make Alarm Strategy for this Counting Task."));
    p_stackedwidget_->addWidget(p_utname_);
    p_stackedwidget_->addWidget(p_uttype_);
    p_stackedwidget_->addWidget(p_utcamera_);
    p_stackedwidget_->addWidget(p_utalarmcounting_);
    p_stackedwidget_->setCurrentIndex(0);

    p_lay_main->setMargin(5);
    p_lay_main->setSpacing(5);
    p_lay_main->addWidget(p_task_list_);
    p_lay_main->addWidget(p_stackedwidget_, 0, Qt::AlignHCenter);
    p_lay_main->setStretchFactor(p_task_list_, 1);
    p_lay_main->setStretchFactor(p_stackedwidget_, 3);
    p_lay_main->setSizeConstraint(QLayout::SetFixedSize);

//! [widget_btn]
    QWidget *p_wgt_btn = new QWidget(this);
    QBoxLayout *p_lay_btn = new QHBoxLayout(p_wgt_btn);
    QPushButton *p_btn_cancel = new QPushButton(tr("Cancel"), p_wgt_btn);
    p_back_ = new QPushButton(tr("<<Previous"), p_wgt_btn);
    p_back_->hide();
    p_next_ = new QPushButton(tr("Next>>"), p_wgt_btn);

    p_lay_btn->addWidget(p_btn_cancel);
    p_lay_btn->addStretch();
    p_lay_btn->addWidget(p_back_);
    p_lay_btn->addWidget(p_next_);

    p_layout->addWidget(p_wgt_main);
    p_layout->addWidget(p_wgt_btn);

    setLayout(p_layout);

//! [setup connection]
    connect(p_stackedwidget_, &QStackedWidget::currentChanged, [=](int index){
        p_task_list_->setCurrentRow(index);
    });
    connect(p_back_, SIGNAL(clicked()), this, SLOT(OnBtnBackClicked()));
    connect(p_next_, SIGNAL(clicked()), this, SLOT(OnBtnNextClicked()));
    connect(p_btn_cancel, SIGNAL(clicked()), this, SLOT(deleteLater()));
}

void TaskWidget::OnBtnBackClicked()
{
    switch (p_stackedwidget_->currentIndex()) {
    case 0: // task name
        break;
    case 1:  // task type
        p_stackedwidget_->setCurrentIndex(0);
        p_back_->hide();
        break;
    case 2:  // task camera
        p_stackedwidget_->setCurrentIndex(1);
        if (task_type_ != kTaskTypeCounting) {
            p_next_->setText("Next>>");
        }
        break;
    case 3:  // task alarm
        p_stackedwidget_->setCurrentIndex(2);
        p_next_->setText("Next>>");
        break;
    }
}

void TaskWidget::OnBtnNextClicked()
{
    switch (p_stackedwidget_->currentIndex()) {
    case 0:  // task name
        if (!set_task_name())
            return;
        task_path_ = kTaskRootPath + task_name_ + "/";
        p_stackedwidget_->setCurrentIndex(1);
        p_back_->show();
        break;
    case 1:  // task type
        if (!set_task_type())
            return;
        p_stackedwidget_->setCurrentIndex(2);
        if (task_type_ != kTaskTypeCounting) {
            p_next_->setText("Finish");
        }
        break;
    case 2: // task camera
        if (!set_task_camera())
            return;
        if (task_type_ == kTaskTypeCounting) {
            p_stackedwidget_->setCurrentIndex(3);
            p_next_->setText("Finish");
        } else {
            OnBtnFinishClicked();
        }
        break;
    case 3: // task alarm
        if (!set_alarm_strategy())
            return;
        OnBtnFinishClicked();
        break;
    }
}

/* set task_name*/
bool TaskWidget::set_task_name()
{
    if (p_utname_->task_name().isEmpty()) {
        p_utname_->set_warning(QString("Task Name can't be Empty!"));
        return false;
    }

    task_name_ = p_utname_->task_name().toStdString();
    if (Utility::IsNameExistInDB("Tasks", "task_name", task_name_)) {
        p_utname_->set_warning(QString("Task Name already Exist!"));
        return false;
    }

    p_utname_->set_warning("");

    return true;
}

/* set task_type */
bool TaskWidget::set_task_type()
{
    task_type_ = p_uttype_->task_type().toStdString();

    return true;
}

/* set task_source */
bool TaskWidget::set_task_camera()
{
    if (p_utcamera_->camera().isEmpty()) {
        p_utcamera_->set_warning(QString("address can't be Empty!"));
        return false;
    }
    if (p_utcamera_->camera_type() == "Video") {
        if (!Utility::IsCameraValid(p_utcamera_->camera().toStdString(), width_, height_, fps_, total_frames_)) {
            p_utcamera_->set_warning(p_utcamera_->camera_type() + QString(" Can't Open!"));
            return false;
        }
        // get video info
        camera_type_ = kCameraTypeFile;
    } else {
        camera_type_ = kCameraTypeHttp;
    }

    camera_ = p_utcamera_->camera().toStdString();
    p_utcamera_->set_warning("");

    return true;
}

/* set alarm strategy */
bool TaskWidget::set_alarm_strategy()
{
    if (p_utalarmcounting_->alarm_switch()) {
        if (p_utalarmcounting_->priority_low().isEmpty() || p_utalarmcounting_->priority_medium().isEmpty() || p_utalarmcounting_->priority_high().isEmpty()) {
            p_utalarmcounting_->set_warning(QString("priority can't be empty!"));
            return false;
        }

        if ((p_utalarmcounting_->priority_low() >= p_utalarmcounting_->priority_medium()) || p_utalarmcounting_->priority_medium() >= p_utalarmcounting_->priority_high()) {
            p_utalarmcounting_->set_warning(QString("priority must satisfy: low < medium < high !"));
            return false;
        }

        alarm_switch_ = kStatusON;
        priority_low_ = p_utalarmcounting_->priority_low().toInt();
        priority_medium_ = p_utalarmcounting_->priority_medium().toInt();
        priority_high_ = p_utalarmcounting_->priority_high().toInt();
    } else {
        alarm_switch_ = kStatusOFF;
    }

    p_utalarmcounting_->set_warning("");

    return true;
}

/* Create Files */
bool TaskWidget::CreateFiles()
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

bool TaskWidget::InsertNewTaskToDB()
{
    if (!DBHelper::InsertToTasks(task_name_, task_type_, task_status_, camera_type_, camera_status_, task_path_, alarm_switch_, report_switch_, group_name_))
        return false;

    if (camera_type_ == kCameraTypeFile) {
        if (!DBHelper::InsertToFiles(camera_, width_, height_, fps_, total_frames_, task_name_)) {
            DBHelper::DeleteFromTable("Tasks", "task_name", task_name_);
            return false;
        }
    } else {
        if (!DBHelper::InsertToTaskCamera(task_name_, camera_)) {
            DBHelper::DeleteFromTable("Tasks", "task_name", task_name_);
            return false;
        }
    }

    if (alarm_switch_ == kStatusON) {
        if (!DBHelper::InsertToDensityAlarmStrategy(priority_low_, priority_medium_, priority_high_, task_name_)) {
            DBHelper::DeleteFromTable("Tasks", "task_name", task_name_);
            return false;
        }
    }

    return true;
}

void TaskWidget::OnBtnFinishClicked()
{
    // Insert To DB When Clicked Finish Button;
    if (!InsertNewTaskToDB()) {
        QMessageBox::critical(NULL, "Create Task", "DB Error!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return;
    }

    /* Create Folder For ROI, Pers, LM, Alarm and GT
      * Delete Task From DB When Create Folder Failed */
    if (!CreateFiles()) {
        DBHelper::DeleteFromTable("Tasks", "task_name", task_name_);
        return;
    }

    emit InsertTaskFinished();
}
