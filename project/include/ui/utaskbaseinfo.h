/****************************************************************************
 *
 *  Copyright (C) 2015 CUHK. All rights reserved.
 *  Author: Liping Zhang
 *
 * ****************************************************************************/

#ifndef UTASKBASEINFO_H
#define UTASKBASEINFO_H

#include <QWidget>
class QLineEdit;
class QLabel;
class QComboBox;

/**
 * @brief The UTaskBaseInfo class
 */
class UTaskBaseInfo : public QWidget
{
    Q_OBJECT
public:
    explicit UTaskBaseInfo(QWidget *parent = 0);
    ~UTaskBaseInfo();

    QString taskname();
    int tasktype();
    void set_warning(QString str);
signals:

public slots:

private:
    QLineEdit *taskname_;
    QComboBox *tasktype_;
    QLabel *warning_;

};

#endif // UTASKBASEINFO_H
