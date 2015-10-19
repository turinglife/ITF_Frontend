/****************************************************************************
 *
 *  Copyright (C) 2015 CUHK. All rights reserved.
 *  Author: Liping Zhang
 *
 * ****************************************************************************/

#ifndef DELEGATE_H
#define DELEGATE_H

#include <QWidget>
#include "buffer.hpp"
#include "taskcommandcenter.h"
#include <QThread>
#include "common.h"
#include "dbi.hpp"
#include <bits/ios_base.h>
#include <QTimer>

class UTaskBoard;
class UClipWidget;
class UCountingSetting;
class UPlotWidget;
class AlarmWidget;

/**
 * @brief The Delegate class
 */
class Delegate : public QWidget
{
    Q_OBJECT
public:
    explicit Delegate(QWidget *parent = 0);
    ~Delegate();

    void Init(std::vector<std::map<std::string, std::string> > res, int channel);

    std::string task_name();
    std::string task_type();
    UClipWidget *src_clip();
    UClipWidget *dst_clip() const;
    UPlotWidget *plot() const;
    AlarmWidget *alarm();


signals:
    void CameraStarted();
    void CameraStoped();
    void TaskStarted();
    void TaskStoped();
    void RemoveFromChannel(int);
    void ThreadIsFinished();
    void ShowAnalyzeBoard(int);

public slots:
    void PlayCamera();
    void StartCD();
    void StartAD();
    void StartRD();
    void StopCD();
    void StopAD();

private slots:
    void CommandAssigner(QString daemon, QString operation);
    void MessageReceiver(QString daemon, QString operation, bool flag);
    void onCountingSettingClicked();

private:
    bool InitBuffer();

    bool IsReadyToStartAD();

    void ReadCountingSetting();
    void WriteCountingSetting();

    bool InsertToDensityDetail();
    bool SelectFromTable(const std::string &table, const std::string &key, const std::string &val, std::vector<std::map<std::string, std::string> > &res);
    bool UpdateStatusToTasks(const std::string &key, const std::string &status);
    void DeleteFromTable(const std::string &table, const std::string &key, const std::string &val);


    std::map<std::string, std::string> task_info_;
    int curr_channel_;

    UClipWidget *src_clip_;
    UClipWidget *dst_clip_;
    UPlotWidget *plot_;
    AlarmWidget *alarm_;

    unsigned int timestamp_;
    cv::Mat curr_src_;
    cv::Mat curr_dst_;
    int curr_count_;

    int frame_width_;
    int frame_height_;
    float frame_sketchpad_rate_w_;
    float frame_sketchpad_rate_h_;

    bool camera_started_;
    bool task_started_;

    QTimer timer;
    QThread thread_;
    CBuffer buffer_;
    TaskCommandCenter *command_center_;
    UCountingSetting *countingsetting_;

    QVector<cv::Point> roi_points_;
};

#endif // DELEGATE_H
