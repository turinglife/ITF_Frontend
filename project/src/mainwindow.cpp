#include "mainwindow.h"
#include "common.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QWidget>
#include <QLayout>
#include "utaskboard.h"
#include "utreewidget.h"
#include <QLineEdit>
#include <QPushButton>
#include <QDebug>
#include <QSplitter>
#include <QMessageBox>
#include "delegate.h"
#include "cameramanager.h"
#include "newtask.h"
#include "utility.h"
#include "unewgroup.h"
#include "utaskalarm.h"
#include "uanalyzeboard.h"
#include "uclipwidget.h"
#include "ucountingsetting.h"
#include "dbi.hpp"
#include "alarmanalyzer.h"
#include "uplotwidget.h"
#include "alarmwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
//! [Set App Window]
    setWindowTitle(tr("ITF Smart Client"));
    setMinimumSize(qApp->desktop()->availableGeometry().size());

//! [Create App Folder]
    if (!Utility::CreateFileFolder(kAppRootPath)) {
        return;
    }
//! [Create Task Folder]
    if (!Utility::CreateFileFolder(kTaskRootPath)) {
        return;
    }

//! [Central Widget]
    QWidget *centralwidget = new QWidget(this);
    setCentralWidget(centralwidget);

//! [StackedWidget]
    QHBoxLayout *layout = new QHBoxLayout;
    stackedwidget_ = new QStackedWidget(this);
    layout->addWidget(stackedwidget_);

//! [TabWidget]
    tabwidget_ = new QTabWidget(this);
    tabwidget_->setTabPosition(QTabWidget::West);
    stackedwidget_->addWidget(tabwidget_);

//! [ UAnalyzeBoard]
    analyzeboard_ = new UAnalyzeBoard(this);
    stackedwidget_->addWidget(analyzeboard_);

//! [Task Module]
    QSplitter *splitter = new QSplitter(Qt::Horizontal, tabwidget_);

    //! [Task Tree]
    tasktree_ = new UTreeWidget(this);
    tasktree_->setHeaderLabel(tr("Task"));

    //! [Task Board]
    taskboard_ = new UTaskBoard(this);

    splitter->addWidget(tasktree_);
    splitter->addWidget(taskboard_);
    splitter->setStretchFactor(0,0);
    splitter->setStretchFactor(1,1);

//! [Alarm Module]
//    UTaskAlarm *lab_alarm  = new UTaskAlarm(this);
    AlarmAnalyzer *alarmanalyzer = new AlarmAnalyzer(this);

    tabwidget_->addTab(splitter, "Task");
    tabwidget_->addTab(alarmanalyzer, "Alarm");

//    QVBoxLayout *layout = new QVBoxLayout;
//    layout->addWidget(tabwidget_);
    centralwidget->setLayout(layout);

    // Load Task Tree
    LoadTaskTree();

    // Recover Analyze Board
    RecoverAnalyzeTask();

//! [QTimer]
    // Start Timer
//    timer_.start(1000/40);

    // Setup Connection
    connect(tasktree_, SIGNAL(CreateGroup()), this, SLOT(CreateGroup()));
    connect(tasktree_, SIGNAL(CreateTask()), this, SLOT(CreateTask()));
    connect(tasktree_, SIGNAL(DeleteGroup()), this, SLOT(DeleteGroup()));
    connect(tasktree_, SIGNAL(DeleteTask()), this, SLOT(DeleteTask()));
    connect(tasktree_, SIGNAL(SelectChannel(QString,int)), this, SLOT(SelectChannel(QString,int)));

    connect(analyzeboard_, SIGNAL(BackToTaskBoard(int)), this, SLOT(ShowTaskBoard(int)));
}

MainWindow::~MainWindow()
{

}

/* Recover Analyze Task */
void MainWindow::RecoverAnalyzeTask()
{
    // Get Info From DB
    std::vector<std::map<std::string, std::string> > Tasks;
    if (!SelectFromTable("Tasks", Tasks)) {
        return;
    }
    if (Tasks.empty()) {
        return;
    }

    int channel = 0;
    for (size_t i=0; i<Tasks.size(); ++i) {
        if (Tasks[i]["camera_status"] == kStatusON) {
            std::vector<std::map<std::string, std::string> > res;
            if (!SelectFromTable("Tasks", "task_name", Tasks[i]["task_name"], res)) {
                return;
            }
            if (res.empty()) {
                return;
            }

            CreateDelegate(res, channel);
            ++channel;
        }
    }
}

void MainWindow::CreateGroup()
{
    UNewGroup *group = new UNewGroup(0);
    group->setFixedSize(kGroupWidth, kGroupHeight);
    Utility::MoveToCenter(group);
    group->show();

    connect(group, &UNewGroup::CreateGroupFinished, [=](QString str){
        // Check Group Name in DB
        if (IsGroupNameExist(str)) {
            group->set_warning("Group Name Already Exist!");
            return;
        }
        if (!InsertToGroups(str)) {
            QMessageBox::critical(NULL, "critical", "Insert To Groups Failed!", QMessageBox::Ok, QMessageBox::Ok);
        }
        group->deleteLater();

        // ReLoad Task Tree
        LoadTaskTree();
    });
}

/* Delete Group From DB and ReLoad TaskTree */
void MainWindow::DeleteGroup()
{
    QMessageBox::StandardButton btn_message = QMessageBox::warning(NULL, "warning", "Delete Group Will Delete All tasks in that group!", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (btn_message == QMessageBox::No) {
        return;
    }

    std::string group_name = tasktree_->currentItem()->text(0).toStdString();

    // Delete Tasks File Folder in that Group
    std::vector<std::map<std::string, std::string> > res;
    if (!SelectFromTable("Tasks", "group_name", group_name, res)) {
        return;
    }

    for (size_t i=0; i<res.size(); ++i) {
        std::string task_path = res[i]["task_path"];
        if (!Utility::RemoveFileFolder(task_path)) {
            return;
        }
    }

    // Delete Group from DB will Delete All Tasks in that Group Automatically
    if (DeleteFromTable("Groups", "group_name", group_name)) {
        // ReLoad Task Tree
        LoadTaskTree();
    }
}

void MainWindow::CreateTask()
{
    QTreeWidgetItem *item = tasktree_->currentItem();
    QString group_name = item->text(0);

    NewTask *task = new NewTask(0);
    task->Init(group_name);
    task->setFixedSize(kTaskWidth, kTaskHeight);
    Utility::MoveToCenter(task);
    task->show();

    connect(task, &NewTask::InsertTaskFinished, [=](){
        // Refresh db for construct task tree
        task->deleteLater();
        // ReLoad Task Tree
        LoadTaskTree();
    });
}

void MainWindow::DeleteTask()
{
    QMessageBox::StandardButton btn_message = QMessageBox::warning(NULL, "warning", "Delete Task!", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (btn_message == QMessageBox::No) {
        return;
    }

    std::string task_name = tasktree_->currentItem()->text(0).toStdString();

    // Delete Task File Folder
    std::vector<std::map<std::string, std::string> > res;
    if (!SelectFromTable("Tasks", "task_name", task_name, res)) {
        return;
    }
    if (res.empty()) {
        return;
    }

    std::string task_path = res[0]["task_path"];

    if (!Utility::RemoveFileFolder(task_path)) {
        return;
    }

    // Delete Task From DB
    if (DeleteFromTable("Tasks", "task_name", task_name)) {
        // ReLoad Task Tree
        LoadTaskTree();
    }
}

/* Select Channel For Task and Create Delegate*/
void MainWindow::SelectChannel(QString str, int channel)
{
    // Get Info From DB
    std::string task_name = str.toStdString();
    std::vector<std::map<std::string, std::string> > res;
    if (!SelectFromTable("Tasks", "task_name", task_name, res)) {
        return;
    }
    if (res.empty()) {
        return;
    }

    // Create A Delegate
    CreateDelegate(res, channel);
}

void MainWindow::CreateDelegate(std::vector<std::map<std::string, std::string> > res, int channel)
{
    // Create a delegate and Init parameters
    delegate_ = new Delegate(this);
    delegate_->Init(res, channel);

    // Set channel for Src
    taskboard_->set_channel(channel, delegate_->src_clip());

    // Set channel unavailabel when using
    tasktree_->set_channel_status(channel, false);

    // Save delegate to Hash
    delegate_hash_.insert(channel, delegate_);

    // Setup connection
//    connect(&timer_, SIGNAL(timeout()), delegate_, SLOT(PlayCamera()));
    connect(delegate_, SIGNAL(RemoveFromChannel(int)), this, SLOT(RemoveFromChannel(int)));
    connect(delegate_, SIGNAL(ShowAnalyzeBoard(int)), this, SLOT(ShowAnalyzeBoard(int)));
}

void MainWindow::RemoveFromChannel(int channel)
{
    // Get delegate from Hash
    delegate_ = delegate_hash_.value(channel);

    // Setup Disconnection
//    disconnect(&timer_, SIGNAL(timeout()), delegate_, SLOT(PlayCamera()));
    disconnect(delegate_, SIGNAL(RemoveFromChannel(int)), this, SLOT(RemoveFromChannel(int)));

    // Remove delegate from Hash
    delegate_hash_.remove(channel);

    // Clear channel
    taskboard_->clear_channel(channel);

    // Set channel availabel
    tasktree_->set_channel_status(channel, true);

    // Delete delegate
    delete delegate_;
}

void MainWindow::ShowTaskBoard(int channel)
{
    // Get delegate
    delegate_ = delegate_hash_.value(channel);

    // Remove Src, Dst and Plot(if any) from AnalyzeBoard
    analyzeboard_->RemoveLayouts();

    // Insert Src to Taskboard
    // recover task_type for src_clip
    delegate_->src_clip()->set_curr_type(analyzeboard_->task_type());
    taskboard_->set_channel(channel, delegate_->src_clip());

    // Show TaskBoard
    stackedwidget_->setCurrentIndex(0);
}

/* Move src_clip from taskboard to analyzeboard */
void MainWindow::ShowAnalyzeBoard(int channel)
{
    // Get delegate
    delegate_ = delegate_hash_.value(channel);

    // Clear channel
    taskboard_->clear_channel(channel);

    // Setup AnalyzeBoard
    // Mark channel and task_type
    analyzeboard_->set_channel(channel);
    analyzeboard_->set_task_name(delegate_->task_name());
    analyzeboard_->set_task_type(delegate_->task_type());

    // Insert Src, Dst and Plot(if any) to AnalyzeBoard
    analyzeboard_->layouts()[1]->addWidget(delegate_->src_clip());
    delegate_->src_clip()->setVisible(true);
    delegate_->src_clip()->set_curr_type("");

    analyzeboard_->layouts()[1]->addWidget(delegate_->dst_clip());
    delegate_->dst_clip()->setVisible(true);

    if (delegate_->task_type() == kTaskTypeCounting) {
        analyzeboard_->layouts()[2]->addWidget(delegate_->plot());
        delegate_->plot()->setVisible(true);
        analyzeboard_->layouts()[0]->addWidget(delegate_->alarm());
        delegate_->alarm()->setVisible(true);
    }

    // Show AnalyzeBoard
    stackedwidget_->setCurrentIndex(1);
}

void MainWindow::LoadTaskTree()
{
    // Reset tree
    tasktree_->clear();

    // Load groups
    if (LoadGroupsInfo()) {
        // Load tasks;
        LoadTasksInfo();
    }
}

bool MainWindow::LoadGroupsInfo()
{
    std::vector<std::map<std::string, std::string> > res;
    if (!SelectFromTable("Groups", res)) {
        return false;
    }
    if (res.empty()) {
        return true;
    }

    for (size_t i=0; i<res.size(); ++i) {
        // Each group item
        QString group_name = QString::fromStdString(res[i]["group_name"]);
        QTreeWidgetItem *item= new QTreeWidgetItem;
        item->setText(0, group_name);
        tasktree_->addTopLevelItem(item);
        qDebug() << group_name;
    }

    return true;
}

bool MainWindow::LoadTasksInfo()
{
    std::vector<std::map<std::string, std::string> > res;
    if (!SelectFromTable("Tasks", res)) {
        return false;
    }
    if (res.empty()) {
        return true;
    }

    for (size_t i=0; i<res.size(); ++i) {
        QString task_name = QString::fromStdString(res[i]["task_name"]);
        QString group_name = QString::fromStdString(res[i]["group_name"]);

        QTreeWidgetItem *item_group;
        for( int j=0; j<tasktree_->topLevelItemCount(); ++j) {
            if (group_name == tasktree_->topLevelItem(j)->text(0)) {
                item_group = tasktree_->topLevelItem(j);
            }
        }

        QTreeWidgetItem *item_task = new QTreeWidgetItem;
        item_task->setText(0, task_name);
        item_group->addChild(item_task);
    }

    return true;
}

bool MainWindow::IsGroupNameExist(QString str)
{
    // Check group_name in DB
    std::vector<std::map<std::string, std::string> > res;
    if (!SelectFromTable("Groups", res)) {
        return false;
    }
    if (res.empty()) {
        return false;
    }

    for (size_t i = 0; i < res.size(); ++i) {
        if (str.toStdString() == res[i]["group_name"]) {
            return true;
        }
    }

    return false;
}

bool MainWindow::InsertToGroups(QString str)
{
    // Connect to DB
    CDbi db;
    if (! Utility::IsDBConnected(db)) {
        QMessageBox::warning(0, "Warning", "Connect DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }

    return db.RunSQL("INSERT INTO Groups VALUES ('" + str.toStdString() + "');");
}

bool MainWindow::DeleteFromTable(const string &table, const string &key, const string &val)
{
    // Connect to DB
    CDbi db;
    if (! Utility::IsDBConnected(db)) {
        QMessageBox::warning(0, "Warning", "Connect DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }

    return db.RunSQL("DELETE FROM " + table + " WHERE " + key + "='" + val + "';");
}

bool MainWindow::SelectFromTable(const string &table, std::vector<std::map<string, string> > &res)
{
    // Connect to DB
    CDbi db;
    if (! Utility::IsDBConnected(db)) {
        QMessageBox::warning(0, "Warning", "Connect DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }

    // Get Info From Table
    res = db.Query("SELECT * FROM " + table + ";");

    return true;
}

bool MainWindow::SelectFromTable(const string &table, const string &key, const string &val, std::vector<std::map<string, string> > &res)
{
    // Connect to DB
    CDbi db;
    if (! Utility::IsDBConnected(db)) {
        QMessageBox::warning(0, "Warning", "Connect DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }

    // Get Info From Table
    res = db.Query("SELECT * FROM " + table + " WHERE " + key + "='" + val + "';");

    return true;
}

void MainWindow::EndApp()
{
}
