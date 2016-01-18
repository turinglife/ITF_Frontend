#include "utcamera.h"
#include <QLayout>
#include "utility.h"
#include "cameramodule.h"

UTCamera::UTCamera(QWidget *parent) : UTBaseWidget(parent)
{
    QVBoxLayout *p_layout = new QVBoxLayout;

    QGridLayout *p_lay_main = new QGridLayout;
    // tasktype
    QLabel *p_lab_1 = new QLabel(tr("Source Type"), this);
    p_camera_type_ = new QComboBox(this);
    p_camera_type_->addItem(tr("Video"));
    p_camera_type_->addItem(tr("Camera"));
    p_camera_type_->setCurrentIndex(0);

    QHBoxLayout *p_lay_camera = new QHBoxLayout;
    QLabel *p_lab_2 = new QLabel(tr("Source"), this);
    p_camera_ = new QLineEdit(this);
    QPushButton *p_btn_open = new QPushButton(tr("Open"), this);
    p_lay_camera->addWidget(p_camera_);
    p_lay_camera->addWidget(p_btn_open);
    p_camera_->setEnabled(false);

    p_lay_main->addWidget(p_lab_1, 0, 0);
    p_lay_main->addWidget(p_camera_type_, 0, 1, 1, 2);
    p_lay_main->addWidget(p_lab_2, 1, 0);
    p_lay_main->addLayout(p_lay_camera, 1, 1, 1, 2);
    p_lay_main->setColumnMinimumWidth(1, 250);

    p_layout->addWidget(p_guide_);
    p_layout->addLayout(p_lay_main);
    p_layout->addWidget(p_warning_);

    setLayout(p_layout);

    //set connection
    connect(p_camera_type_, SIGNAL(currentTextChanged(QString)), this, SLOT(CurrentTextChanged(QString)));
    connect(p_btn_open, SIGNAL(clicked()), this, SLOT(OnBtnOpenClicked()));
}

UTCamera::~UTCamera()
{

}

void UTCamera::CurrentTextChanged(QString)
{
    p_camera_->clear();
    p_warning_->clear();
}

void UTCamera::OnBtnOpenClicked()
{
    if (p_camera_type_->currentText().trimmed() == QString("Video")) {
        QString filename = Utility::GetFileName("video(*.mp4 *.avi *.mkv)");
        p_camera_->setText(filename);
    } else {
        // [CameraModule]
        CameraModule *p_camera_module = new CameraModule(0);
        p_camera_module->set_flag(true, false, false, false);
        p_camera_module->setMinimumSize(kMinAppWidth, kMinAppHeight);
        Utility::MoveToCenter(p_camera_module);
        p_camera_module->show();

        connect(p_camera_module, &CameraModule::CameraSelected, [=](QString qstr) {
            p_camera_->setText(qstr);
            p_camera_module->deleteLater();
        });
    }
}

