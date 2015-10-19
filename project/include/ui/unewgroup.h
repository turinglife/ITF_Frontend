#ifndef UNEWGROUP_H
#define UNEWGROUP_H

#include <QDialog>

class QLabel;
class QLineEdit;

class UNewGroup : public QDialog
{
    Q_OBJECT
public:
    explicit UNewGroup(QWidget *parent = 0);
    ~UNewGroup();

    void set_warning(QString str);
signals:
    void CreateGroupFinished(QString);

private slots:
    void On_Btn_Finish_Clicked();

private:
    QLabel *warning_;
    QLineEdit *group_name_;
};

#endif // UNEWGROUP_H
