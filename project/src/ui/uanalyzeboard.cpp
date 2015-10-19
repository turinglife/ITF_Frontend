#include "uanalyzeboard.h"
#include <QSplitter>
#include <QLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>

UAnalyzeBoard::UAnalyzeBoard(QWidget *parent)
    : QWidget(parent)
{
    //! [Left Layout]
    QGroupBox *gbox_left = new QGroupBox(this);
    QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(gbox_left->sizePolicy().hasHeightForWidth());
    gbox_left->setSizePolicy(sizePolicy1);
    gbox_left->setMinimumSize(QSize(220, 0));
    QVBoxLayout *lay_left = new QVBoxLayout(gbox_left);
    lay_left->setSpacing(0);
    lay_left->setContentsMargins(5, 5, 5, 5);

    QHBoxLayout *lay_btn_back = new QHBoxLayout;
    QPushButton *btn_back = new QPushButton(tr("Back"), this);
    connect(btn_back, SIGNAL(clicked()), this, SLOT(OnBtnBackClicked()));

    lay_btn_back->addWidget(btn_back);
    lay_btn_back->addStretch();

    QHBoxLayout *lay_taskname = new QHBoxLayout;
    QLabel *lab_taskname = new QLabel(tr("TaskName:"), this);
    task_name_ = new QLabel(this);
    QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Fixed);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(task_name_->sizePolicy().hasHeightForWidth());
    task_name_->setSizePolicy(sizePolicy2);
    lab_taskname->setBuddy(task_name_);

    lay_taskname->addWidget(lab_taskname);
    lay_taskname->addWidget(task_name_);

    QHBoxLayout *lay = new QHBoxLayout;
    lay->setSpacing(0);
    layouts_.push_back(lay);

    lay_left->addLayout(lay_btn_back);
    lay_left->addSpacing(10);
    lay_left->addLayout(lay_taskname);
    lay_left->addLayout(lay);
    lay_left->addStretch();

    //! [Right Layout]
    QGroupBox *gbox_right = new QGroupBox(this);
    QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(gbox_right->sizePolicy().hasHeightForWidth());
    gbox_right->setSizePolicy(sizePolicy3);

    QVBoxLayout *lay_right = new QVBoxLayout(gbox_right);
    lay_right->setSpacing(0);
    lay_right->setContentsMargins(0, 0, 0, 0);

    for (int i=0; i<2; ++i) {
       QHBoxLayout *lay = new QHBoxLayout;
       lay->setSpacing(0);

       lay_right->addLayout(lay);

       layouts_.push_back(lay);
    }

    QVBoxLayout *layout = new QVBoxLayout;

    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(gbox_left);
    splitter->addWidget(gbox_right);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);

    layout->addWidget(splitter);

    setLayout(layout);
}

UAnalyzeBoard::~UAnalyzeBoard()
{

}

void UAnalyzeBoard::set_channel(int channel)
{
    channel_ = channel;
}

void UAnalyzeBoard::set_task_name(std::string task_name)
{
    task_name_->setText(QString::fromStdString(task_name));
}

void UAnalyzeBoard::set_task_type(std::string type)
{
    task_type_ = type;
}

std::string UAnalyzeBoard::task_type()
{
    return task_type_;
}

void UAnalyzeBoard::RemoveLayouts()
{
    for (int i=0; i<layouts_.size(); ++i) {
        QLayoutItem *child;
        while ((child = layouts_[i]->takeAt(0)) != 0) {
           child->widget()->setVisible(false);
           delete child;
        }
    }

    task_name_->clear();
}

QVector<QLayout *> UAnalyzeBoard::layouts()
{
    return layouts_;
}

void UAnalyzeBoard::OnBtnBackClicked()
{
    emit BackToTaskBoard(channel_);
}

