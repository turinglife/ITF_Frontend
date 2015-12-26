#ifndef UTCAMERA_H
#define UTCAMERA_H

#include "utbasewidget.h"
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>

class UTCamera : public UTBaseWidget
{
    Q_OBJECT
public:
    explicit UTCamera(QWidget *parent = 0);
    ~UTCamera();

    inline QString camera_type() const {return p_camera_type_->currentText().trimmed();}
    inline QString camera() const {return p_camera_->text().trimmed();}
    inline void set_camera(QString qstr) {p_camera_->setText(qstr);}

signals:

private slots:
    void CurrentTextChanged(QString);
    void OnBtnOpenClicked();

private:
    QComboBox *p_camera_type_;
    QLineEdit *p_camera_;
};

#endif // UTCAMERA_H
