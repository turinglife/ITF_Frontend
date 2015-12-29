#include "uclipanalysisboard.h"
#include <QSplitter>
#include <QLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include "utility.h"

UClipAnalysisBoard::UClipAnalysisBoard(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *p_layout = new QVBoxLayout(this);

//! [Left Layout]
    QGroupBox *p_gbox_left = new QGroupBox(this);
    Utility::SetWdgSizePolicy(p_gbox_left, QSizePolicy::Minimum, QSizePolicy::Preferred);
    p_gbox_left->setMinimumSize(QSize(220, 0));
    QVBoxLayout *p_lay_left = new QVBoxLayout(p_gbox_left);
    p_lay_left->setSpacing(0);
    p_lay_left->setContentsMargins(5, 5, 5, 5);

    QHBoxLayout *p_lay_btn= new QHBoxLayout;
    QPushButton *p_btn_back = new QPushButton(tr("Back"), this);
    connect(p_btn_back, SIGNAL(clicked()), this, SLOT(OnBtnBackClicked()));
    p_lay_btn->addWidget(p_btn_back);
    p_lay_btn->addStretch();

    QHBoxLayout *p_lay_taskname = new QHBoxLayout;
    QLabel *p_lab_1 = new QLabel(tr("TaskName:"), this);
    p_task_name_ = new QLabel(this);
    Utility::SetWdgSizePolicy(p_task_name_, QSizePolicy::Minimum, QSizePolicy::Fixed);
    p_lab_1->setBuddy(p_task_name_);

    p_lay_taskname->addWidget(p_lab_1);
    p_lay_taskname->addWidget(p_task_name_);

    QHBoxLayout *lay = new QHBoxLayout;
    lay->setSpacing(0);
    layouts_.push_back(lay);

    p_lay_left->addLayout(p_lay_btn);
    p_lay_left->addSpacing(10);
    p_lay_left->addLayout(p_lay_taskname);
    p_lay_left->addLayout(lay);
    p_lay_left->addStretch();

//! [Right Layout]
    QGroupBox *p_gbox_right = new QGroupBox(this);
    Utility::SetWdgSizePolicy(p_gbox_right, QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *p_lay_right = new QVBoxLayout(p_gbox_right);
    p_lay_right->setSpacing(0);
    p_lay_right->setContentsMargins(0, 0, 0, 0);

    for (int i=0; i<2; ++i) {
       QHBoxLayout *lay = new QHBoxLayout;
       lay->setSpacing(0);

       p_lay_right->addLayout(lay);

       layouts_.push_back(lay);
    }

    QSplitter *p_splitter = new QSplitter(Qt::Horizontal, this);
    p_splitter->addWidget(p_gbox_left);
    p_splitter->addWidget(p_gbox_right);
    p_splitter->setStretchFactor(0, 0);
    p_splitter->setStretchFactor(1, 1);

    p_layout->addWidget(p_splitter);
}

UClipAnalysisBoard::~UClipAnalysisBoard()
{

}

void UClipAnalysisBoard::set_task_name(string task_name)
{
    p_task_name_->setText(QString::fromStdString(task_name));
}

void UClipAnalysisBoard::RemoveLayouts()
{
    for (int i=0; i<layouts_.size(); ++i) {
        QLayoutItem *child;
        while ((child = layouts_[i]->takeAt(0)) != 0) {
           child->widget()->setVisible(false);
           delete child;
        }
    }

    p_task_name_->clear();
}

QVector<QLayout *> UClipAnalysisBoard::layouts()
{
    return layouts_;
}

void UClipAnalysisBoard::OnBtnBackClicked()
{
    emit BackToClipBoard(channel_);
}

