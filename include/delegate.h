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
#include "uclipwidget.h"

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

    UClipWidget *src_clip();
    UClipWidget *dst_clip_1() const;
    UClipWidget *dst_clip_2() const;
    UPlotWidget *plot() const;
    UClipAlarmWidget *clip_alarm();

signals:
    void CameraStarted();
    void CameraStoped();
    void TaskStarted();
    void TaskStoped();
    void ThreadIsFinished();
    void RemoveFromChannel(int);
    void ShowClipAnalysisBoard(int);

public slots:
    void StartCD();
    void StopCD();
    void StartAD();
    void StopAD();
    void StartRD();
    void StartMD();
    void StopMD();

private slots:
    void PlaySrc();
    void PlayDst();
    void CommandAssigner(QString daemon, QString operation);
    void MessageReceiver(QString daemon, QString operation, bool flag);
    void OnCameraStarted();
    void OnTaskStarted();
    void OnTaskStoped();
    void OnThreadStarted();
    void OnThreadFInished();
    void OnStartADClicked();
    void OnStopADclicked();
    void OnShowClipAnalysisBoardClicked();
    void OnCountingSettingClicked();
    void OnRemoveFromBoardClicked();

private:
    bool InitBuffer();
    bool IsReadyToStartAD();
    bool VideoIsFinished();
    bool ReadCountingSetting();
    void WriteCountingSetting();
    void ConnectStatus(bool flag);

    void ConvertToSlice(const cv::Mat &src, cv::Mat &dst, cv::Point pt1, cv::Point pt2);
    void ExtractLine(const cv::Mat &src, cv::Point pt1, cv::Point pt2, vector<cv::Vec3b> &line);
    void OrthogonalLine(const cv::Point &src_pt1, const cv::Point &src_pt2, const cv::Point &dst_pt1, cv::Point &dst_line_pt2, int dst_line_length);
    void ArrowedLine(cv::Mat &img, cv::Point pt1, cv::Point pt2, const cv::Scalar& color, int thickness=1, int line_type=8, int shift=0, double tipLength=0.5);
    void release_memory();

    std::map<std::string, std::string> task_info_;
    int curr_channel_;

    UClipWidget *p_src_clip_;
    UClipWidget *p_dst_clip_1_;
    UClipWidget *p_dst_clip_2_;
    UPlotWidget *p_plot_;
    UClipAlarmWidget *p_clip_alarm_;

    unsigned int curr_timestamp_;
    cv::Mat curr_src_;
    cv::Mat curr_dst_1_;
    cv::Mat curr_dst_2_;
    cv::Mat curr_dst_slice_1_;
    cv::Mat curr_dst_slice_2_;
    int curr_count_1_;
    int curr_count_2_;

    int fps_;
    int frame_width_;
    int frame_height_;
    float frame_ratio_;
    int slice_width_;
    int slice_height_;

    bool alarm_started_;
    bool video_finished_;

    QTimer timer_src_;
    QTimer timer_dst_;
    QThread thread_;
    CBuffer buffer_;
    TaskCommandCenter *p_command_center_;
    CountingSetting *p_countingsetting_;

    QVector<cv::Point> roi_points_;
    QVector<cv::Point> arrow_points_;
};

#endif // DELEGATE_H
