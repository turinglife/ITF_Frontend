#include "intelligentvideoanalysis.h"
#include <QStackedWidget>
#include "alarmmodule.h"
#include "cameramodule.h"
#include "analysismodule.h"
#include "utility.h"
#include <QCloseEvent>

IntelligentVideoAnalysis::IntelligentVideoAnalysis(QWidget *parent)
    : QMainWindow(parent)
{
//! [Set App Window]
    setWindowTitle(tr("Intelligent Video Analysis"));

    setMinimumSize(kMinAppWidth, kMinAppHeight);

    Utility::MoveToCenter(this);
//! [Create App Folder]
    if (!Utility::CreateFileFolder(kAppRootPath))
        return;
//! [Create Task Folder]
    if (!Utility::CreateFileFolder(kTaskRootPath))
        return;

//! [Central Widget]
    QWidget *centralwidget = new QWidget(this);
    setCentralWidget(centralwidget);

//! [StackedWidget]
    QHBoxLayout *p_layout = new QHBoxLayout(centralwidget);
    p_stackedwidget_ = new QStackedWidget(this);
    p_layout->addWidget(p_stackedwidget_);

//! [Analysis Module]
    p_analysis_module_ = new AnalysisModule(this);
    p_stackedwidget_->addWidget(p_analysis_module_);

//! [Alarm Module]
    p_alarm_module_ = new AlarmModule(this);
    p_stackedwidget_->addWidget(p_alarm_module_);

//! [Camera Moduel]
    p_camera_module_ = new CameraModule(this);
    p_camera_module_->set_flag(false, true, true, true);
    p_stackedwidget_->addWidget(p_camera_module_);

//! [Action]
    QAction *p_act_analysis_module = new QAction(tr("Analysis"), this);
    QAction *p_act_alarm_module = new QAction(tr("Alarm"), this);
    QAction *p_act_camera_module = new QAction(tr("Camera"), this);
    connect(p_act_analysis_module, &QAction::triggered, [=](){p_stackedwidget_->setCurrentIndex(0);});
    connect(p_act_alarm_module, &QAction::triggered, [=](){
        p_alarm_module_->set_clear();
        p_stackedwidget_->setCurrentIndex(1);});
    connect(p_act_camera_module, &QAction::triggered, [=](){p_stackedwidget_->setCurrentIndex(2);});

    p_toolbar_ = addToolBar("ToolBar");
    p_toolbar_->addAction(p_act_analysis_module);
    p_toolbar_->addAction(p_act_alarm_module);
    p_toolbar_->addAction(p_act_camera_module);
}

IntelligentVideoAnalysis::~IntelligentVideoAnalysis()
{
    std::cout << "release IntelligentVideoAnalysis" << std::endl;
}

void IntelligentVideoAnalysis::closeEvent(QCloseEvent *event)
{
    if (QMessageBox::No == QMessageBox::question(NULL, "Information", "Close Application?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No)) {
        event->ignore();
        return;
    }
}

void IntelligentVideoAnalysis::SetCurrentIndex(int index)
{
    p_stackedwidget_->setCurrentIndex(index);
}
