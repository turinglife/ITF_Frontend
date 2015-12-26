#include "groupwidget.h"
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QValidator>
#include "utility.h"

GroupWidget::GroupWidget(QWidget *parent) : UTBaseWidget(parent)
{
//! [window setting]
    setWindowTitle("New Group");
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);
    Utility::MoveToCenter(this);

    QVBoxLayout *p_layout = new QVBoxLayout(this);

    QHBoxLayout *p_lay_main = new QHBoxLayout;
    QLabel *p_lab_1 = new QLabel(tr("Name"), this);
    p_group_name_ = new QLineEdit(this);

    QRegExp regExp;
    regExp.setPatternSyntax(QRegExp::RegExp);
    regExp.setCaseSensitivity(Qt::CaseSensitive);
    regExp.setPattern(QString("^[A-Za-z0-9_]+$"));
    QRegExpValidator *p_validator = new QRegExpValidator (regExp,this);
    p_group_name_->setValidator(p_validator);

    p_lay_main->setMargin(5);
    p_lay_main->setSpacing(5);
    p_lay_main->addWidget(p_lab_1);
    p_lay_main->addWidget(p_group_name_);

    QHBoxLayout *p_lay_btn = new QHBoxLayout;
    QPushButton *p_btn_finish = new QPushButton(tr("Finish"), this);
    p_lay_btn->addStretch();
    p_lay_btn->addWidget(p_btn_finish);

    p_layout->addWidget(p_guide_);
    p_layout->addLayout(p_lay_main);
    p_layout->addWidget(p_warning_);
    p_layout->addLayout(p_lay_btn);

    connect(p_btn_finish, SIGNAL(clicked()), this, SLOT(OnBtnFinishClicked()));
}

GroupWidget::~GroupWidget()
{

}

void GroupWidget::OnBtnFinishClicked()
{
    p_warning_->setText("");

    if (p_group_name_->text().isEmpty()) {
        p_warning_->setText("Group Name Can't be Empty!");
        return;
    }

    if (Utility::IsNameExistInDB("Groups", "group_name", p_group_name_->text().toStdString())) {
        p_warning_->setText("Group Name Already Exist!");
        return;
    }

    if (!DBHelper::InsertToGroups(p_group_name_->text().toStdString())) {
        QMessageBox::critical(NULL, "Create Groups", "Failed!", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    QMessageBox::information(NULL, "Create Groups", "Success!", QMessageBox::Ok, QMessageBox::Ok);

    emit GroupCreated();
}

