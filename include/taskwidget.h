#ifndef TASKWIDGET_H
#define TASKWIDGET_H

#include <QWidget>

class UTName;
class UTType;
class UTCamera;
class UTAlarmCounting;
class QListWidget;
class QStackedWidget;
class QPushButton;
class QListWidget;

class TaskWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TaskWidget(QWidget *parent = 0);
    ~TaskWidget();

    void Init(QString group_name);

signals:
    void InsertTaskFinished();

private slots:
    void OnBtnBackClicked();
    void OnBtnNextClicked();

private:
    bool set_task_name();
    bool set_task_type();
    bool set_task_camera();
    bool set_alarm_strategy();

    bool CreateFiles();
    bool InsertNewTaskToDB();

    QListWidget *p_task_list_;
    QStackedWidget *p_stackedwidget_;
    UTName *p_utname_;
    UTType *p_uttype_;
    UTCamera *p_utcamera_;
    UTAlarmCounting *p_utalarmcounting_;
    QPushButton *p_back_;
    QPushButton *p_next_;


    // Table of Tasks
    std::string task_name_;
    std::string task_type_;
    std::string task_status_;
    std::string camera_type_;
    std::string camera_status_;
    std::string task_path_;
    std::string alarm_switch_;
    std::string report_switch_;
    std::string group_name_;

    // Table of Task_Camera
    // Table of Files;
    std::string camera_;
    int width_;
    int height_;
    int fps_;
    int total_frames_;

    // Table of DensityAlarmStrategy
    int priority_low_;
    int priority_medium_;
    int priority_high_;
};

#endif // TASKWIDGET_H
