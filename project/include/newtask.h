#ifndef NEWTASK_H
/****************************************************************************
 *
 *  Copyright (C) 2015 CUHK. All rights reserved.
 *  Author: Liping Zhang
 *
 * ****************************************************************************/

#define NEWTASK_H

#include <QDialog>

class QStackedWidget;
class UTaskName;
class UTaskType;
class UTaskCamera;
class UTaskAlarm;
class UTaskReport;
class QListWidget;
class QPushButton;
class CameraManager;

/**
 * @brief The NewTask class
 */
class NewTask : public QDialog
{
    Q_OBJECT
public:
    explicit NewTask(QWidget *parent = 0);
    ~NewTask();

    void Init(QString group_name);

signals:
    void InsertTaskFinished();

private slots:
    void on_btnBack_clicked();
    void on_btnNext_clicked();

private:
    bool set_task_name();
    bool set_task_type();
    bool set_task_source();
    bool set_alarm_strategy();

    bool CreateFiles();
    bool IsTaskNameExist(QString str);
    bool InsertNewTaskToDB();
    bool InsertToTasks();
    bool InsertToTaskCamera();
    bool InsertToFiles();
    bool InsertToDensityAlarmStrategy();
    void DeleteTask();

    QListWidget *task_list_;
    QStackedWidget *stackedwidget_;
    UTaskName *ui_taskname_;
    UTaskType *ui_tasktype_;
    UTaskCamera *ui_taskcamera_;
    UTaskAlarm *ui_taskalarm_;
    UTaskReport *ui_taskreport_;
    QPushButton *btn_back_;
    QPushButton *btn_next_;

    CameraManager *camera_manager_;

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
    std::string camera_name_;

    // Table of Files;
    std::string file_url_;
    int width_;
    int height_;
    int fps_;
    int total_frames_;

    // Table of DensityAlarmStrategy
    int priority_low_;
    int priority_medium_;
    int priority_high_;

};

#endif // NEWTASK_H
