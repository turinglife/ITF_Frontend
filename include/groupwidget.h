#ifndef GROUPWIDGET_H
#define GROUPWIDGET_H

#include "utbasewidget.h"
class QLineEdit;

class GroupWidget : public UTBaseWidget
{
    Q_OBJECT
public:
    explicit GroupWidget(QWidget *parent = 0);
    ~GroupWidget();

signals:
    void GroupCreated();

private slots:
    void OnBtnFinishClicked();

private:
    QLineEdit *p_group_name_;
};

#endif // GROUPWIDGET_H
