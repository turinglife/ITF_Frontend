/****************************************************************************
 *
 *  Copyright (C) 2015 CUHK. All rights reserved.
 *  Author: Liping Zhang
 *
 * ****************************************************************************/

#ifndef UTASKCAMERA_H
#define UTASKCAMERA_H

#include <QWidget>
class QComboBox;
class QLineEdit;
class QLabel;
class QPushButton;

/**
 * @brief The UTaskCamera class
 */
class UTaskCamera : public QWidget
{
    Q_OBJECT
public:
    explicit UTaskCamera(QWidget *parent = 0);
    ~UTaskCamera();

    QString camera_type();
    QString address();
    void set_warning(QString str);
    void set_camera(QString);

    QPushButton *btn_camera_;


signals:

private slots:
    void currentTextChanged(QString str);

private:
    QComboBox *camera_type_;

    QLineEdit *video_;
    QPushButton *btn_video_;

    QLineEdit *camera_;

    QLabel *warning_;
};

#endif // UTASKCAMERA_H
