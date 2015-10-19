#ifndef ALARMWIDGET_H
#define ALARMWIDGET_H

#include <QWidget>

class QLabel;
class LightWidget;

class AlarmWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AlarmWidget(QWidget *parent = 0);
    ~AlarmWidget();

    void set_alarm_strategy(bool on, std::string low, std::string medium, std::string high);

    void set_value(double);
signals:

private slots:
    void OnAlarmBtnClicked();

private:
    bool alarm_on_;

    int low_;
    int medium_;
    int high_;

    LightWidget *green_;
    LightWidget *yellow_;
    LightWidget *red_;
};

#endif // ALARMWIDGET_H
