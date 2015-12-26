#ifndef UTALARMCOUNTING_H
#define UTALARMCOUNTING_H

#include "utalarmstrategy.h"
#include <QCheckBox>
#include <QLineEdit>

class UTAlarmCounting : public UTAlarmStrategy
{
    Q_OBJECT
public:
    explicit UTAlarmCounting(QWidget *parent = 0);
    ~UTAlarmCounting();

    inline bool alarm_switch() const {return p_alarm_switch_->isChecked();}
    inline QString priority_low() const {return p_priority_low_->text();}
    inline QString priority_medium() const {return p_priority_medium_->text();}
    inline QString priority_high() const {return p_priority_high_->text();}

signals:

private slots:
    void OnCheckBoxClicked();

private:
    QCheckBox *p_alarm_switch_;
    QLineEdit *p_priority_low_;
    QLineEdit *p_priority_medium_;
    QLineEdit *p_priority_high_;
};

#endif // UTALARMCOUNTING_H
