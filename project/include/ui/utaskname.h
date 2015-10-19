/****************************************************************************
 *
 *  Copyright (C) 2015 CUHK. All rights reserved.
 *  Author: Liping Zhang
 *
 * ****************************************************************************/

#ifndef UTASKNAME_H
#define UTASKNAME_H

#include <QWidget>
class QLineEdit;
class QLabel;

/**
 * @brief The UTaskName class
 */
class UTaskName : public QWidget
{
    Q_OBJECT
public:
    explicit UTaskName(QWidget *parent = 0);
    ~UTaskName();

    QString taskname();
    void set_warning(QString str);

signals:

public slots:

private:
    QLineEdit *taskname_;
    QLabel *warning_;
};

#endif // UTASKNAME_H
