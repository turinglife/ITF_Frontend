#include "utreewidget.h"
#include <QMenu>

UTreeWidget::UTreeWidget(QWidget *parent) : QTreeWidget(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));

//! [Create New Group]
    create_group_ = new QAction(tr("New Group"), this);
    connect(create_group_, &QAction::triggered, [=](){
        emit CreateGroup();
    });

//! [Delete New Group]
    delete_group_ = new QAction(tr("Delete Group"), this);
    connect(delete_group_, &QAction::triggered, [=](){
        emit DeleteGroup();
    });
//    delete_group_->setEnabled(false);

//! [Create New Task]
    create_task_ = new QAction(tr("New Task"), this);
    connect(create_task_, &QAction::triggered, [=](){
        emit CreateTask();
    });
//! [Delete Task]
    delete_task_ = new QAction(tr("Delete Task"), this);
    connect(delete_task_, &QAction::triggered, [=](){
        emit DeleteTask();
    });
//    delete_task_->setEnabled(false);

//! [channel setting]
    for (int i=0; i<9; ++i) {
        channels_.append(new QAction(QString("channel%1").arg(i), this));
        connect(channels_.last(), &QAction::triggered, [=](){
            emit SelectChannel(this->currentItem()->text(0), i);
        });
    }

}

UTreeWidget::~UTreeWidget()
{
}

void UTreeWidget::set_channel_status(int channel, bool flag)
{
    channels_.at(channel)->setEnabled(flag);
}

void UTreeWidget::onCustomContextMenuRequested(const QPoint &pos)
{
    // add Popup Menu to Right Click.
    QTreeWidgetItem *item = itemAt(pos);
    QMenu *menu = new QMenu(this);

    if (item) {
        if (!item->parent()) {
            menu->addAction(create_task_);
            menu->addAction(delete_group_);
        } else {
            QMenu *sub_menu = new QMenu(this);
            sub_menu->setTitle("select channel");
            for (int i=0; i<channels_.size(); i++) {
                sub_menu->addAction(channels_.at(i));
            }
            menu->addMenu(sub_menu);
            menu->addAction(delete_task_);
        }

    } else {
        menu->addAction(create_group_);
    }

    menu->popup(mapToGlobal(pos));
}
