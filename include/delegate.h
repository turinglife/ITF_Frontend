#ifndef DELEGATE_H
#define DELEGATE_H

#include <QWidget>
#include <QTimer>
#include "buffer.hpp"
#include "taskcommandcenter.h"
#include <QThread>
#include "common.h"
#include <bits/ios_base.h>

#include "uplotwidget.h"
#include "countingsetting.h"
#include "uclipalarmwidget.h"


#ifdef USE_GLWIDGET
#include "uclipglwidget.h"
#else
#include "uclipwidget.h"
#endif

class Delegate : public QWidget
{
    Q_OBJECT
public:
    explicit Delegate(QWidget *parent = 0);
    ~Delegate();

    void Init(std::vector<std::map<std::string, std::string> > task_info, int channel);
    inline std::map<std::string, std::string> task_info() const {
        return task_info_;
    }

#ifdef USE_GLWIDGET
#include "uclipglwidget.h"
    UClipGLWidget *src_clip();
    UClipGLWidget *dst_clip() const;
#else
    UClipWidget *src_clip();
    UClipWidget *dst_clip() const;
#endif
    UPlotWidget *plot() const;
    UClipAlarmWidget *clip_alarm();


signals:
    void CameraStarted();
    void CameraStoped();
    void TaskStarted();
    void TaskStoped();
    void RemoveFromChannel(int);
    void ThreadIsFinished();
    void ShowClipAnalysisBoard(int);

public slots:
    void StartCD();
    void StartAD();
    void StopCD();
    void StopAD();
    void StartRD();
    void StartMD();
    void StopMD();

private slots:
    void PlayCamera();
    void CommandAssigner(QString daemon, QString operation);
    void MessageReceiver(QString daemon, QString operation, bool flag);
    void OnCameraStarted();
    void OnTaskStarted();
    void OnTaskStoped();
    void OnThreadFInished();
    void OnShowClipAnalysisBoardClicked();
    void OnCountingSettingClicked();

private:
    bool InitBuffer();
    bool IsReadyToStartAD();
    bool VideoIsFinished();
    bool ReadCountingSetting();
    void WriteCountingSetting();

    std::map<std::string, std::string> task_info_;
    int curr_channel_;

#ifdef USE_GLWIDGET
#include "uclipglwidget.h"
    UClipGLWidget *p_src_clip_;
    UClipGLWidget *p_dst_clip_;
#else
    UClipWidget *p_src_clip_;
    UClipWidget *p_dst_clip_;
#endif

    UPlotWidget *p_plot_;
    UClipAlarmWidget *p_clip_alarm_;

    unsigned int timestamp_;
    cv::Mat curr_src_;
    cv::Mat curr_dst_;
    int curr_count_;

    int fps_;
    int frame_width_;
    int frame_height_;
    float frame_sketchpad_rate_w_;
    float frame_sketchpad_rate_h_;

    bool camera_started_;
    bool task_started_;
    bool alarm_started_;
    bool video_finished_;

    QTimer timer_;
    QThread thread_;
    CBuffer buffer_;
    TaskCommandCenter *p_command_center_;
    CountingSetting *p_countingsetting_;

    QVector<cv::Point> roi_points_;
};

#endif // DELEGATE_H
