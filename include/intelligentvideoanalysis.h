#ifndef INTELLIGENTVIDEOANALYSIS_H
#define INTELLIGENTVIDEOANALYSIS_H

#include <QMainWindow>
class QStackedWidget;
class AnalysisModule;
class AlarmModule;
class CameraModule;

class IntelligentVideoAnalysis : public QMainWindow
{
    Q_OBJECT
public:
    explicit IntelligentVideoAnalysis(QWidget *parent = 0);
    ~IntelligentVideoAnalysis();

signals:

public slots:

protected:
//    void paintEvent(QPaintEvent *event);

private slots:
    void SetCurrentIndex(int index);

private:
    QToolBar *p_toolbar_;
    QStackedWidget *p_stackedwidget_;
    AnalysisModule *p_analysis_module_;
    AlarmModule *p_alarm_module_;
    CameraModule *p_camera_module_;
};

#endif // INTELLIGENTVIDEOANALYSIS_H
