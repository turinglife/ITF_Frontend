#include "unewgroup.h"
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QValidator>

UNewGroup::UNewGroup(QWidget *parent) : QDialog(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;

    QHBoxLayout *lay_main = new QHBoxLayout;
    QLabel *lab_group = new QLabel(tr("Name"), this);
    group_name_ = new QLineEdit(this);

    QRegExp regExp;
    regExp.setPatternSyntax(QRegExp::RegExp);
    regExp.setCaseSensitivity(Qt::CaseSensitive);
    regExp.setPattern(QString("^[A-Za-z0-9_]+$"));
    QRegExpValidator *validator = new QRegExpValidator (regExp,this);
    group_name_->setValidator(validator );

    lay_main->setMargin(5);
    lay_main->setSpacing(5);
    lay_main->addWidget(lab_group);
    lay_main->addWidget(group_name_);

    QHBoxLayout *lay_warning = new QHBoxLayout;
    warning_ = new QLabel(this);
    warning_->setStyleSheet("color:rgb(255,0,0)");
    lay_warning->addWidget(warning_);

    QHBoxLayout *lay_btn = new QHBoxLayout;
    QPushButton *btn_finish = new QPushButton(tr("Finish"), this);
    lay_btn->addStretch();
    lay_btn->addWidget(btn_finish);

    layout->addLayout(lay_main);
    layout->addLayout(lay_warning);
    layout->addLayout(lay_btn);

    setLayout(layout);

    connect(btn_finish, SIGNAL(clicked()), this, SLOT(On_Btn_Finish_Clicked()));
}

UNewGroup::~UNewGroup()
{

}

void UNewGroup::set_warning(QString str)
{
    warning_->setText(str);
}

void UNewGroup::On_Btn_Finish_Clicked()
{
    warning_->setText("");

    if (group_name_->text().isEmpty()) {
        warning_->setText("Group Name Can't be Empty!");
        return;
    }

    emit CreateGroupFinished(group_name_->text());
}
