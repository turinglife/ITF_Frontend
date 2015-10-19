#include "utaskcamera.h"
#include <QLabel>
#include <QLineEdit>
#include <QLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QComboBox>
#include <QDebug>
#include "utility.h"

UTaskCamera::UTaskCamera(QWidget *parent) :
    QWidget(parent)
{
    // Layout
    QVBoxLayout *layout = new QVBoxLayout;

    // some introduction about this widget
    QVBoxLayout *lay_intro = new QVBoxLayout;
    QLabel *lab_intro = new QLabel(this);
    lab_intro->setText(tr("camera setting."));
    lab_intro->setWordWrap(true);
    lay_intro->addWidget(lab_intro);

    QLabel *lab_cameratype = new QLabel(tr("Camera Type:"), this);
    camera_type_ = new QComboBox(this);
    camera_type_->addItem(tr("Video"));
    camera_type_->addItem(tr("Camera"));
    camera_type_->setCurrentIndex(0);

    QHBoxLayout *lay_video = new QHBoxLayout;
    QLabel *lab_video = new QLabel(tr("Video:"), this);
    video_ = new QLineEdit(this);
    btn_video_ = new QPushButton(tr("Select Video..."), this);
    lay_video->addWidget(video_);
    lay_video->addWidget(btn_video_);
    video_->setEnabled(true);
    btn_video_->setEnabled(true);

    QHBoxLayout *lay_camera = new QHBoxLayout;
    QLabel* lab_camera = new QLabel(tr("Camera:"), this);
    camera_ = new QLineEdit(this);
    btn_camera_ = new QPushButton(tr("Select Camera"), this);
    lay_camera->addWidget(camera_);
    lay_camera->addWidget(btn_camera_);
    camera_->setEnabled(false);
    btn_camera_->setEnabled(false);

    QGridLayout *lay_main = new QGridLayout;
    lay_main->addWidget(lab_cameratype, 0, 0);
    lay_main->addWidget(camera_type_, 0, 1, 1, 2);
    lay_main->addWidget(lab_video, 1, 0);
    lay_main->addLayout(lay_video, 1, 1, 1, 3);
    lay_main->addWidget(lab_camera, 2, 0);
    lay_main->addLayout(lay_camera, 2, 1, 1, 3);

    // show message when task name wrong
    QVBoxLayout *lay_warning = new QVBoxLayout;
    warning_ = new QLabel(this);
    warning_->setStyleSheet("color:rgb(255,0,0)");
    lay_warning->addWidget(warning_);

    layout->addLayout(lay_intro);
    layout->addLayout(lay_main);
    layout->addLayout(lay_warning);

    setLayout(layout);

    // set connection
    connect(camera_type_, SIGNAL(currentTextChanged(QString)), this, SLOT(currentTextChanged(QString)));
    connect(btn_video_, &QPushButton::clicked, [=](){
        QString filename = Utility::GetFileName("video(*.mp4 *.avi *.mkv)");
        video_->setText(filename);
    });
}


UTaskCamera::~UTaskCamera()
{

}

QString UTaskCamera::camera_type()
{
    return camera_type_->currentText().trimmed();
}

QString UTaskCamera::address()
{
    if (camera_type_->currentText() == "Video")
        return video_->text().trimmed();
    else if (camera_type_->currentText() == "Camera")
        return camera_->text().trimmed();
    else
        return "";
}

void UTaskCamera::set_warning(QString str)
{
    warning_->setText(str);
}

void UTaskCamera::set_camera(QString str)
{
    camera_->setText(str);
}

void UTaskCamera::currentTextChanged(QString str)
{
    video_->clear();
    camera_->clear();
    warning_->clear();
    if (str == "Video") {
        video_->setEnabled(true);
        btn_video_->setEnabled(true);
        camera_->setEnabled(false);
        btn_camera_->setEnabled(false);

    } else if (str == "Camera") {
        video_->setEnabled(false);
        btn_video_->setEnabled(false);
        camera_->setEnabled(true);

        btn_camera_->setEnabled(true);
    }
}
