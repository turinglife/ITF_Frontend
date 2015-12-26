#include "utname.h"
#include <QLayout>
#include <QValidator>

UTName::UTName(QWidget *parent) : UTBaseWidget(parent)
{
    QVBoxLayout *p_layout = new QVBoxLayout;

    QGridLayout *p_lay_main = new QGridLayout;
    //input taskname
    QLabel *p_lab_1 = new QLabel(tr("Task Name: "), this);
    p_taskname_ = new QLineEdit(this);
    p_taskname_->setClearButtonEnabled(true);

    QRegExp regExp;
    regExp.setPatternSyntax(QRegExp::RegExp);
    regExp.setCaseSensitivity(Qt::CaseSensitive);
    regExp.setPattern(QString("^[A-Za-z0-9_]+$"));
    QRegExpValidator *validator = new QRegExpValidator (regExp, this);
    p_taskname_->setValidator(validator);

    p_lay_main->addWidget(p_lab_1, 0, 0);
    p_lay_main->addWidget(p_taskname_, 0, 1);
    p_lay_main->setColumnMinimumWidth(1, 250);

    p_layout->addWidget(p_guide_);
    p_layout->addLayout(p_lay_main);
    p_layout->addWidget(p_warning_);

    setLayout(p_layout);
}

UTName::~UTName()
{

}

