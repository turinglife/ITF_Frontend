/****************************************************************************
 *
 *  Copyright (C) 2015 CUHK. All rights reserved.
 *  Author: Liping Zhang
 *
 * ****************************************************************************/

#ifndef UTASKREPORT_H
#define UTASKREPORT_H

#include <QWidget>

class QCheckBox;

/**
 * @brief The UTaskReport class
 */
class UTaskReport : public QWidget
{
    Q_OBJECT
public:
    explicit UTaskReport(QWidget *parent = 0);
    ~UTaskReport();

    bool report_switch();

signals:

public slots:

private:
    QCheckBox *report_switch_;
};

#endif // UTASKREPORT_H
