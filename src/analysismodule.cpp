#include "analysismodule.h"
#include "taskwidget.h"
#include "groupwidget.h"
#include "utasktree.h"
#include "utility.h"
#include "delegate.h"
#include "uclipboard.h"
#include "uclipanalysisboard.h"
#include <QStackedWidget>
#include <QLayout>

AnalysisModule::AnalysisModule(QWidget *parent) : QWidget(parent)
{

    QHBoxLayout *p_layout = new QHBoxLayout(this);
    p_stackedwidget_ = new QStackedWidget(this);
    p_layout->addWidget(p_stackedwidget_);

    QSplitter *p_splitter = new QSplitter(Qt::Horizontal, p_stackedwidget_);

//! [Task Tree]
    p_task_tree_ = new UTaskTree(this);
    p_task_tree_->setHeaderLabel(tr("Task"));
//! [Clip Board]
    p_clip_board_ = new UClipBoard(this);
    p_splitter->addWidget(p_task_tree_);
    p_splitter->addWidget(p_clip_board_);
    p_splitter->setStretchFactor(0,0);
    p_splitter->setStretchFactor(1,1);

    p_stackedwidget_->addWidget(p_splitter);

//! [UClipAnalysisBoard]
    p_clip_analysis_board_ = new UClipAnalysisBoard(this);
    p_stackedwidget_->addWidget(p_clip_analysis_board_);

    // Setup Connection
    connect(p_task_tree_, SIGNAL(CreateGroup()), this, SLOT(CreateGroup()));
    connect(p_task_tree_, SIGNAL(CreateTask()), this, SLOT(CreateTask()));
    connect(p_task_tree_, SIGNAL(DeleteGroup()), this, SLOT(DeleteGroup()));
    connect(p_task_tree_, SIGNAL(DeleteTask()), this, SLOT(DeleteTask()));
    connect(p_task_tree_, SIGNAL(SelectChannel(QString,int)), this, SLOT(SelectChannel(QString,int)));

    connect(p_clip_analysis_board_, SIGNAL(BackToClipBoard(int)), this, SLOT(ShowClipBoard(int)));

    Init();
}

AnalysisModule::~AnalysisModule()
{
    std::cout << "release AnalysisModule" << std::endl;
    release_memory();
}

void AnalysisModule::Init()
{
    // Load Task Tree
    LoadTaskTree();
    // Recover Analyze Board
    RecoverAnalysisTasks();
}

void AnalysisModule::CreateGroup()
{
    GroupWidget *p_group = new GroupWidget(NULL);
    p_group->setFixedSize(kGroupWidth, kGroupHeight);
    Utility::MoveToCenter(p_group);
    p_group->show();

    connect(p_group, &GroupWidget::GroupCreated, [=](){
        p_group->deleteLater();
        // ReLoad Task Tree
        LoadTaskTree();
    });

}

void AnalysisModule::CreateTask()
{
    QTreeWidgetItem *p_item = p_task_tree_->currentItem();
    QString group_name = p_item->text(0);
    TaskWidget *p_task = new TaskWidget(NULL);
    p_task->Init(group_name);
    p_task->setFixedSize(kTaskWidth, kTaskHeight);
    Utility::MoveToCenter(p_task);
    p_task->show();

    connect(p_task, &TaskWidget::InsertTaskFinished, [=](){
        // Refresh db for construct task tree
        p_task->deleteLater();
        // ReLoad Task Tree
        LoadTaskTree();
    });
}

void AnalysisModule::DeleteGroup()
{
    QMessageBox::StandardButton btn_message = QMessageBox::warning(NULL, "warning", "Delete Group Will Delete All tasks in that group!", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (btn_message == QMessageBox::No)
        return;

    string group_name = p_task_tree_->currentItem()->text(0).toStdString();
    // Delete Tasks File Folder in that Group
    std::vector<std::map<std::string, std::string> > res;
    if (!DBHelper::SelectFromTable("Tasks", "group_name", group_name, res))
        return;
    if (res.empty())
        return;

    // Check whethe some Tasks are still running in this group
    for (size_t i=0; i<res.size(); ++i) {
        if (res[i]["camera_status"] == kStatusON) {
            QString qmessage = "Task " + QString::fromStdString(res[i]["task_name"]) + " is still running, please stop it first!";
            QMessageBox::warning(NULL, "warning", qmessage, QMessageBox::Ok , QMessageBox::Ok);
            return;
        }
    }

    // if all Tasks are not running in this group, then can Delete this group
    for (size_t i=0; i<res.size(); ++i) {
        string task_path = res[i]["task_path"];
        if (!Utility::RemoveFileFolder(task_path))
            return;
    }
    // Delete Group from DB will Delete All Tasks in that Group Automatically
    if (!DBHelper::DeleteFromTable("Groups", "group_name", group_name))
        return;

    // ReLoad Task Tree
    LoadTaskTree();
}

void AnalysisModule::DeleteTask()
{
    QMessageBox::StandardButton btn_message = QMessageBox::warning(NULL, "warning", "Delete Task!", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (btn_message == QMessageBox::No) {
        return;
    }

    std::string task_name = p_task_tree_->currentItem()->text(0).toStdString();


    std::vector<std::map<std::string, std::string> > res;
    if (!DBHelper::SelectFromTable("Tasks", "task_name", task_name, res))
        return;
    // Check this task is still running
    if(res[0]["camera_status"] == kStatusON) {
        QString qmessage = "Task " + QString::fromStdString(task_name) + " still running, please stop it first!";
        QMessageBox::warning(NULL, "warning", qmessage, QMessageBox::Ok , QMessageBox::Ok);
        return;
    }

    // Delete Task File Folder
    std::string task_path = res[0]["task_path"];
    if (!Utility::RemoveFileFolder(task_path))
        return;
    // Delete Task From DB
    if (!DBHelper::DeleteFromTable("Tasks", "task_name", task_name))
        return;

    // ReLoad Task Tree
    LoadTaskTree();
}

void AnalysisModule::SelectChannel(QString qstr, int channel)
{
    // Get Info From DB
    std::string task_name = qstr.toStdString();
    std::vector<std::map<std::string, std::string> > res;
    if (!DBHelper::SelectFromTable("Tasks", "task_name", task_name, res))
        return;
    if (res.empty())
        return;

    // Create A Delegate
    CreateDelegate(res, channel);
}

void AnalysisModule::RemoveFromChannel(int channel)
{
    //Get delegate from Hash
    p_delegate_ = delegate_hash_.value(channel);

    // Setup Disconnection
    disconnect(p_delegate_, SIGNAL(RemoveFromChannel(int)), this, SLOT(RemoveFromChannel(int)));
    disconnect(p_delegate_, SIGNAL(ShowClipAnalysisBoard(int)), this, SLOT(ShowClipAnalysisBoard(int)));

    // Remove delegate from Hash
    delegate_hash_.remove(channel);

    // Clear
    p_clip_board_->clear_channel(channel);

    // Set channel availabel
    p_task_tree_->set_channel_status(channel, true);

    // Delete delegate
    delete p_delegate_;
}

void AnalysisModule::ShowClipBoard(int channel)
{
    // Get delegate
    p_delegate_ = delegate_hash_.value(channel);

    // Remove Src, Dst and Plot(if any) from ClipAnalysisBoard
    p_clip_analysis_board_->RemoveLayouts();
    // Insert Src to Clipboard
    // recover task_type for src_clip
    p_delegate_->src_clip()->set_curr_type(p_clip_analysis_board_->task_type());
    p_delegate_->src_clip()->setVisible(true);
    p_delegate_->dst_clip_1()->setVisible(false);
    p_delegate_->dst_clip_2()->setVisible(false);
    p_delegate_->plot()->setVisible(false);
    p_delegate_->clip_alarm()->setVisible(false);

    p_clip_board_->set_channel(channel, p_delegate_->src_clip());

    // Show TaskBoard
    p_stackedwidget_->setCurrentIndex(0);
}

void AnalysisModule::ShowClipAnalysisBoard(int channel)
{
    // Get delegate
    p_delegate_ = delegate_hash_.value(channel);

    // Clear channel
    p_clip_board_->clear_channel(channel);

    // Setup ClipAnalysisBoard
    // Mark channel and task_type
    p_clip_analysis_board_->set_channel(channel);
    p_clip_analysis_board_->set_task_name(p_delegate_->task_info()["task_name"]);
    p_clip_analysis_board_->set_task_type(p_delegate_->task_info()["task_type"]);

    // Insert Src, Dst and Plot(if any) to ClipAnalysisBoard
    p_clip_analysis_board_->layouts()[1]->addWidget(p_delegate_->src_clip());
    p_delegate_->src_clip()->setVisible(true);
    p_delegate_->src_clip()->set_curr_type("");

    if (p_delegate_->task_info()["task_type"] == kTaskTypeCounting) {
        p_clip_analysis_board_->layouts()[0]->addWidget(p_delegate_->clip_alarm());
        p_delegate_->clip_alarm()->setVisible(true);
        p_clip_analysis_board_->layouts()[1]->addWidget(p_delegate_->dst_clip_1());
        p_delegate_->dst_clip_1()->setVisible(true);
        p_clip_analysis_board_->layouts()[2]->addWidget(p_delegate_->plot());
        p_delegate_->plot()->setVisible(true);
    } else if (p_delegate_->task_info()["task_type"] == kTaskTypeCrossline) {
//        p_clip_analysis_board_->layouts()[1]->addWidget(p_delegate_->dst_clip_1());
//        p_delegate_->dst_clip_1()->setVisible(true);
        p_clip_analysis_board_->layouts()[2]->addWidget(p_delegate_->dst_clip_1());
        p_delegate_->dst_clip_1()->setVisible(true);
        p_clip_analysis_board_->layouts()[1]->addWidget(p_delegate_->dst_clip_2());
        p_delegate_->dst_clip_2()->setVisible(true);
        p_clip_analysis_board_->layouts()[2]->addWidget(p_delegate_->plot());
        p_delegate_->plot()->setVisible(true);
    } else {
        p_clip_analysis_board_->layouts()[1]->addWidget(p_delegate_->dst_clip_1());
        p_delegate_->dst_clip_1()->setVisible(true);
    }

    // Show ClipAnalysisBoard
    p_stackedwidget_->setCurrentIndex(1);
}

void AnalysisModule::RecoverAnalysisTasks()
{
    // Get Info From DB
    std::vector<std::map<std::string, std::string> > Tasks;
    if (!DBHelper::SelectFromTable("Tasks", Tasks))
        return;
    if (Tasks.empty())
        return;

    int channel = 0;
    for (size_t i=0; i<Tasks.size(); ++i) {
        if (Tasks[i]["camera_status"] == kStatusON) {
            std::vector<std::map<std::string, std::string> > res;
            if (!DBHelper::SelectFromTable("Tasks", "task_name", Tasks[i]["task_name"], res))
                return;
            if (res.empty())
                return;

            CreateDelegate(res, channel);
            ++channel;
        }
    }
}

void AnalysisModule::LoadTaskTree()
{
    // Reset tree
    p_task_tree_->clear();

    // Load groups
    if (LoadGroupsInfo()) {
        // Load tasks;
        LoadTasksInfo();
    }
}

bool AnalysisModule::LoadGroupsInfo()
{
    std::vector<std::map<std::string, std::string> > res;
    if (!DBHelper::SelectFromTable("Groups", res))
        return false;
    if (res.empty())
        return true;

    for (size_t i=0; i<res.size(); ++i) {
        // Each group item
        QString group_name = QString::fromStdString(res[i]["group_name"]);
        QTreeWidgetItem *p_item= new QTreeWidgetItem;
        p_item->setText(0, group_name);
        p_task_tree_->addTopLevelItem(p_item);
    }

    return true;
}

bool AnalysisModule::LoadTasksInfo()
{
    std::vector<std::map<std::string, std::string> > res;
    if (!DBHelper::SelectFromTable("Tasks", res))
        return false;
    if (res.empty())
        return true;

    for (size_t i=0; i<res.size(); ++i) {
        QString task_name = QString::fromStdString(res[i]["task_name"]);
        QString group_name = QString::fromStdString(res[i]["group_name"]);

        QTreeWidgetItem *p_item_group;
        for( int j=0; j<p_task_tree_->topLevelItemCount(); ++j) {
            if (group_name == p_task_tree_->topLevelItem(j)->text(0)) {
                p_item_group = p_task_tree_->topLevelItem(j);
            }
        }

        QTreeWidgetItem *p_item_task = new QTreeWidgetItem;
        p_item_task->setText(0, task_name);
        p_item_group->addChild(p_item_task);
    }

    return true;
}

void AnalysisModule::CreateDelegate(std::vector<std::map<string, string> > &res, int channel)
{
    // Create a delegate and Init parameters
    p_delegate_ = new Delegate(this);
    p_delegate_->Init(res, channel);

    // Set channel for Src
    p_clip_board_->set_channel(channel, p_delegate_->src_clip());
    // Set channel unavailabel when using
    p_task_tree_->set_channel_status(channel, false);

    // Save delegate to Hash
    delegate_hash_.insert(channel, p_delegate_);

    // Setup connection
    connect(p_delegate_, SIGNAL(RemoveFromChannel(int)), this, SLOT(RemoveFromChannel(int)));
    connect(p_delegate_, SIGNAL(ShowClipAnalysisBoard(int)), this, SLOT(ShowClipAnalysisBoard(int)));
}

void AnalysisModule::release_memory()
{
    for (int i=0; i<9; i++) {
        if (!delegate_hash_.contains(i))
            continue;
        p_delegate_ = delegate_hash_.value(i);
        delegate_hash_.remove(i);
        delete p_delegate_;
    }
}
