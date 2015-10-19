/****************************************************************************
 *
 *  Copyright (C) 2015 CUHK. All rights reserved.
 *  Author: Liping Zhang
 *
 * ****************************************************************************/

#ifndef UTASKTYPE_H
#define UTASKTYPE_H

#include <QWidget>

class QLabel;
class QComboBox;

/**
 * @brief The UTaskType class
 */
class UTaskType : public QWidget
{
    Q_OBJECT
public:
    explicit UTaskType(QWidget *parent = 0);
    ~UTaskType();

    QString tasktype();
    void set_warning(QString str);

signals:

public slots:

private:
    QComboBox *tasktype_;
    QLabel *warning_;
};

#endif // UTASKTYPE_H
