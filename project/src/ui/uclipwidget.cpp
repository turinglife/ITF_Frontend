#include "uclipwidget.h"
#include <QLayout>
#include <QAction>
#include <QMenu>
#include <QMouseEvent>
#include <QPushButton>
#include "common.h"

UClipWidget::UClipWidget(QWidget *parent)
    : QLabel(parent)
{
    setMouseTracking(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), SLOT(onCustomContextMenuRequested(QPoint)));

    // Action
    start_ = new QAction(tr("Start"), this);

    connect(start_, &QAction::triggered, [=](){
        emit Command(QString::fromStdString(kDaemonAD), QString::fromStdString(kCommandStart));
    });

    stop_ = new QAction(tr("Stop"), this);
    connect(stop_, &QAction::triggered, [=](){
        emit Command(QString::fromStdString(kDaemonAD), QString::fromStdString(kCommandStop));
    });

    clear_ = new QAction(tr("Clear"), this);
    connect(clear_, &QAction::triggered, [=](){
        emit RemoveTaskFromBoard();
    });

    countingsetting_ = new QAction(tr("Setting"), this);
    connect(countingsetting_, &QAction::triggered, [=](){
        emit CountingSetting();
    });

    // Layout
    QVBoxLayout *layout = new QVBoxLayout;
    control_ = new QWidget(this);
    control_->setStyleSheet("border:2px groove gray;border-radius:10px;");
    control_->hide();

    QHBoxLayout *lay_control = new QHBoxLayout;
    QPushButton *btn_start = new QPushButton(tr("Start"), this);
    btn_start->setStyleSheet("color:red; background-color:rgb(200,155,100); border:1px groove gray;border-radius:10px;padding:2px 4px;");
    connect(btn_start, &QPushButton::clicked, [=](){
        emit Command(QString::fromStdString(kDaemonAD), QString::fromStdString(kCommandStart));
    });

    QPushButton *btn_detail = new QPushButton(tr("Analyze"), this);
    btn_detail->setStyleSheet("color:red; background-color:rgb(200,155,100); border:1px groove gray;border-radius:10px;padding:2px 4px;");
    connect(btn_detail, &QPushButton::clicked, [=](){
        emit ShowAnalyzeBoard();
    });

    lay_control->addWidget(btn_start);
    lay_control->addWidget(btn_detail);

    control_->setLayout(lay_control);

    layout->addStretch();
    layout->addWidget(control_);

    setLayout(layout);
}

UClipWidget::~UClipWidget()
{

}

void UClipWidget::set_curr_type(const std::string &type)
{
    curr_type_ =  type;
}

void UClipWidget::set_curr_name(const string &name)
{
    curr_name_ = name;
}

void UClipWidget::set_start_enabled(bool flag)
{
    start_->setEnabled(flag);
}

void UClipWidget::set_stop_enabled(bool flag)
{
   stop_->setEnabled(flag);
}

void UClipWidget::set_clear_enabled(bool flag)
{
   clear_->setEnabled(flag);
}

void UClipWidget::set_countingsetting_enabled(bool flag)
{
   countingsetting_->setEnabled(flag);
}

void UClipWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (curr_type_ == kTaskTypeCounting || curr_type_ == kTaskTypeSegmentation || curr_type_ == kTaskTypeStationary) {
        QRect rect = geometry();
        int active_area = rect.height() * 0.8;
        if (event->pos().y() > active_area)
            control_->show();
        else
            control_->hide();
    }
}

void UClipWidget::leaveEvent(QEvent *)
{
    if (curr_type_ == kTaskTypeCounting || curr_type_ == kTaskTypeSegmentation || curr_type_ == kTaskTypeStationary) {
        control_->hide();
    }
}

void UClipWidget::onCustomContextMenuRequested(const QPoint &pos)
{
    if (curr_type_ == kTaskTypeCounting) {
        // Add Popup Menu to Right Click Action;
        QMenu *menu_1 = new QMenu(this);
        menu_1->addAction(start_);
        menu_1->addAction(stop_);
        menu_1->addAction(countingsetting_);
        menu_1->addAction(clear_);

        menu_1->popup(mapToGlobal(pos));

    } else if (curr_type_ == kTaskTypeSegmentation || curr_type_ == kTaskTypeStationary) {
        QMenu *menu_1 = new QMenu(this);
        menu_1->addAction(start_);
        menu_1->addAction(stop_);
        menu_1->addAction(clear_);

        menu_1->popup(mapToGlobal(pos));
    } else {
        return;
    }
}

