#ifndef UCLIPALARMWIDGET_H
#define UCLIPALARMWIDGET_H

#include <QWidget>

class ULightWidget;

class UClipAlarmWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UClipAlarmWidget(QWidget *parent = 0);
    ~UClipAlarmWidget();

    void set_alarm_strategy(bool on, std::string low, std::string medium, std::string high);

    void set_value(double value);
signals:

private:
    bool alarm_on_;

    int low_;
    int medium_;
    int high_;

    ULightWidget *p_light_widget_1_;
    ULightWidget *p_light_widget_2_;
    ULightWidget *p_light_widget_3_;
};

#endif // UCLIPALARMWIDGET_H
