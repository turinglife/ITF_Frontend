#include "utasktree.h"
#include <QMenu>

UTaskTree::UTaskTree(QWidget *parent) : QTreeWidget(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(OnCustomContextMenuRequested(QPoint)));

//! [Create New Group]
    p_create_group_ = new QAction(tr("New Group"), this);
    connect(p_create_group_, &QAction::triggered, [=](){
        emit CreateGroup();
    });

//! [Delete New Group]
    p_delete_group_ = new QAction(tr("Delete Group"), this);
    connect(p_delete_group_, &QAction::triggered, [=](){
        emit DeleteGroup();
    });
//    p_delete_group_->setEnabled(false);

//! [Create New Task]
    p_create_task_ = new QAction(tr("New Task"), this);
    connect(p_create_task_, &QAction::triggered, [=](){
        emit CreateTask();
    });
//! [Delete Task]
    p_delete_task_ = new QAction(tr("Delete Task"), this);
    connect(p_delete_task_, &QAction::triggered, [=](){
        emit DeleteTask();
    });
//    p_delete_task_->setEnabled(false);

//! [channel setting]
    for (int i=0; i<9; ++i) {
        channels_.append(new QAction(QString("channel%1").arg(i), this));
        connect(channels_.last(), &QAction::triggered, [=](){
            emit SelectChannel(this->currentItem()->text(0), i);
        });
    }
}

UTaskTree::~UTaskTree()
{

}

void UTaskTree::set_channel_status(int channel, bool flag)
{
    channels_.at(channel)->setEnabled(flag);
}

void UTaskTree::OnCustomContextMenuRequested(const QPoint &pos)
{
    // add Popup Menu to Right Click.
    QTreeWidgetItem *p_item = itemAt(pos);
    QMenu *menu = new QMenu(this);

    if (p_item) {
        if (!p_item->parent()) {
            menu->addAction(p_create_task_);
            menu->addAction(p_delete_group_);
        } else {
            QMenu *p_sub_menu = new QMenu(this);
            p_sub_menu->setTitle("select channel");
            for (int i=0; i<channels_.size(); i++) {
                p_sub_menu->addAction(channels_.at(i));
            }
            menu->addMenu(p_sub_menu);
            menu->addAction(p_delete_task_);
        }

    } else {
        menu->addAction(p_create_group_);
    }

    menu->popup(mapToGlobal(pos));
}

