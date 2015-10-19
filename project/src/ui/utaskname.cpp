#include "utaskname.h"
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QValidator>

UTaskName::UTaskName(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;

    // some introduction about this widget
    QVBoxLayout *lay_intro = new QVBoxLayout;
    QLabel *lab_intro = new QLabel(this);
    lab_intro->setText(tr("Create New Counting Task. Please Input the Task Name ."));
    lab_intro->setWordWrap(true);
    lay_intro->addWidget(lab_intro);

    QGridLayout *lay_main = new QGridLayout;
    // input task name
    QLabel *lab_taskname = new QLabel(tr("Task Name: "), this);
    taskname_ = new QLineEdit(this);
    taskname_->setClearButtonEnabled(true);

    QRegExp regExp;
    regExp.setPatternSyntax(QRegExp::RegExp);
    regExp.setCaseSensitivity(Qt::CaseSensitive);
    regExp.setPattern(QString("^[A-Za-z0-9_]+$"));
    QRegExpValidator *validator = new QRegExpValidator (regExp,this);
    taskname_->setValidator(validator );

    lay_main->addWidget(lab_taskname, 0 , 0);
    lay_main->addWidget(taskname_, 0 ,1);
    lay_main->setColumnMinimumWidth(1, 250);

    // show message when task name wrong
    QVBoxLayout *lay_warning = new QVBoxLayout;
    warning_ = new QLabel(this);
    warning_->setStyleSheet("color:rgb(255,0,0)");
    lay_warning->addWidget(warning_);

    layout->addLayout(lay_intro);
    layout->addLayout(lay_main);
    layout->addLayout(lay_warning);

    setLayout(layout);
}

UTaskName::~UTaskName()
{

}

QString UTaskName::taskname()
{
    return taskname_->text().trimmed();
}

void UTaskName::set_warning(QString str)
{
    warning_->setText(str);
}

