/****************************************************************************
 *
 *  Copyright (C) 2015 CUHK. All rights reserved.
 *  Author: Liping Zhang
 *
 * ****************************************************************************/

#ifndef UTASKALARM_H
#define UTASKALARM_H

#include <QWidget>
class QCheckBox;
class QLineEdit;
class QLabel;

/**
 * @brief The UTaskAlarm class
 */
class UTaskAlarm : public QWidget
{
    Q_OBJECT
public:
    explicit UTaskAlarm(QWidget *parent = 0);
    ~UTaskAlarm();

    bool alarm_switch();
    int priority_low();
    int priority_medium();
    int priority_high();
    void set_warning(QString str);

signals:

private slots:
    void on_CheckBox_clicked();

private:
    QCheckBox *alarm_switch_;
    QLineEdit *priority_low_;
    QLineEdit *priority_medium_;
    QLineEdit *priority_high_;
    QLabel *warning_;
};

#endif // UTASKALARM_H
