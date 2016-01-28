#include "cameramodule.h"
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QTableView>
#include <QHeaderView>
#include <QLineEdit>
#include <QApplication>
#include <QDesktopWidget>
#include "tablemodel.h"
#include "dbhelper.h"
#include "utility.h"

CameraModule::CameraModule(QWidget *parent) : QWidget(parent)
{
    // Setup UI
    setWindowTitle("Camera Center");
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);

    //! [UI]
    QVBoxLayout *p_layout = new QVBoxLayout;
    p_layout->setSpacing(10);
    p_layout->setContentsMargins(1, 1, 1, 1);

    QWidget *p_wgt_main = new QWidget(this);
    QHBoxLayout *p_lay_wgt_main = new QHBoxLayout(p_wgt_main);
    p_lay_wgt_main->setSpacing(5);
    p_lay_wgt_main->setContentsMargins(5, 5, 5, 5);

    //! [tableview]
    p_tableview_ = new QTableView(p_wgt_main);
    p_tableview_->horizontalHeader()->setStretchLastSection(true);
    p_tableview_->setAlternatingRowColors(true);
    p_tableview_->setSelectionBehavior(QAbstractItemView::SelectRows);
    p_tableview_->setSelectionMode(QAbstractItemView::SingleSelection);
    p_lay_wgt_main->addWidget(p_tableview_);

    QGroupBox *p_gbox_right = new QGroupBox(this);
    QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(p_gbox_right->sizePolicy().hasHeightForWidth());
    p_gbox_right->setSizePolicy(sizePolicy1);
    p_gbox_right->setMinimumSize(QSize(220, 0));

    QVBoxLayout *p_lay_gbox_right = new QVBoxLayout(p_gbox_right);

    QLabel *p_lab_1 = new QLabel(tr("Camera Name"), p_gbox_right);
    p_lay_gbox_right->addWidget(p_lab_1);

    p_cameraname_ = new QLineEdit(p_gbox_right);
    QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Fixed);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(p_cameraname_->sizePolicy().hasHeightForWidth());
    p_cameraname_->setSizePolicy(sizePolicy2);
    p_lay_gbox_right->addWidget(p_cameraname_);

    QLabel *p_lab_2 = new QLabel(tr("Address"), p_gbox_right);
    p_lay_gbox_right->addWidget(p_lab_2);

    p_address_ = new QLineEdit(p_gbox_right);
    sizePolicy2.setHeightForWidth(p_address_->sizePolicy().hasHeightForWidth());
    p_address_->setSizePolicy(sizePolicy2);
    p_lay_gbox_right->addWidget(p_address_);

    QLabel *p_lab_3 = new QLabel(tr("Host"), p_gbox_right);
    p_lay_gbox_right->addWidget(p_lab_3);

    p_host_ = new QLineEdit(p_gbox_right);
    sizePolicy2.setHeightForWidth(p_host_->sizePolicy().hasHeightForWidth());
    p_host_->setSizePolicy(sizePolicy2);
    p_lay_gbox_right->addWidget(p_host_);

    QLabel *p_lab_4 = new QLabel(tr("Port"), p_gbox_right);
    p_lay_gbox_right->addWidget(p_lab_4);

    p_port_ = new QLineEdit(p_gbox_right);
    sizePolicy2.setHeightForWidth(p_port_->sizePolicy().hasHeightForWidth());
    p_port_->setSizePolicy(sizePolicy2);
    p_lay_gbox_right->addWidget(p_port_);

    QLabel *p_lab_5 = new QLabel(tr("Username"), p_gbox_right);
    p_lay_gbox_right->addWidget(p_lab_5);

    p_username_ = new QLineEdit(p_gbox_right);
    sizePolicy2.setHeightForWidth(p_username_->sizePolicy().hasHeightForWidth());
    p_username_->setSizePolicy(sizePolicy2);
    p_lay_gbox_right->addWidget(p_username_);

    QLabel *p_lab_6 = new QLabel(tr("Password"), p_gbox_right);
    p_lay_gbox_right->addWidget(p_lab_6);

    p_password_ = new QLineEdit(p_gbox_right);
    sizePolicy2.setHeightForWidth(p_password_->sizePolicy().hasHeightForWidth());
    p_password_->setSizePolicy(sizePolicy2);
    p_lay_gbox_right->addWidget(p_password_);

    p_select_ = new QPushButton(tr("Done"), p_gbox_right);
    p_select_->setCursor(QCursor(Qt::PointingHandCursor));
    p_lay_gbox_right->addWidget(p_select_);

    p_add_ = new QPushButton(tr("Add Camera"), p_gbox_right);
    p_add_->setCursor(QCursor(Qt::PointingHandCursor));
    p_lay_gbox_right->addWidget(p_add_);

    p_delete_ = new QPushButton(tr("Delete Camera"), p_gbox_right);
    p_delete_->setCursor(QCursor(Qt::PointingHandCursor));
    p_lay_gbox_right->addWidget(p_delete_);

    QSpacerItem *p_verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    p_lay_gbox_right->addItem(p_verticalSpacer);

    p_lay_wgt_main->addWidget(p_gbox_right);

    p_layout->addWidget(p_wgt_main);

    setLayout(p_layout);

    // Setup Connection
    connect(p_tableview_, SIGNAL(clicked(QModelIndex)), this, SLOT(OnTableViewPressed(QModelIndex)));
    connect(p_select_, SIGNAL(clicked()), this, SLOT(OnBtnSelectClicked()));
    connect(p_add_, SIGNAL(clicked()), this, SLOT(OnBtnAddClicked()));
    connect(p_delete_, SIGNAL(clicked()), this, SLOT(OnBtnDeleteClicked()));

    // Setup model
    p_camera_model_ = new TableModel(this);

    // Load Camera Info
    LoadCameraInfo();
}

CameraModule::~CameraModule()
{

}

void CameraModule::set_flag(bool f_select, bool f_add, bool f_delete, bool f_lineedit)
{
    p_select_->setVisible(f_select);
    p_add_->setVisible(f_add);
    p_delete_->setVisible(f_delete);
    p_cameraname_->setEnabled(f_lineedit);
    p_address_->setEnabled(f_lineedit);
    p_host_->setEnabled(f_lineedit);
    p_port_->setEnabled(f_lineedit);
    p_username_->setEnabled(f_lineedit);
    p_password_->setEnabled(f_lineedit);
}

void CameraModule::OnBtnSelectClicked()
{
    // Get Camera Info From Right Side
    std::string camera_name, address, host, port, username, password;
    CameraInfo(camera_name, address, host, port, username, password);
    if (camera_name.empty() || address.empty() || host.empty() || port.empty() || username.empty() || password.empty()) {
        QMessageBox::warning(NULL, "warning", "Please click the leftside list to choose a camera!", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    // test camera
    std::string camera_path = Utility::CameraAddress(username, password, host, port, address);
    if (! Utility::IsCameraValid(camera_path)) {
        QMessageBox::warning(NULL, "warning", "Camera Unvalid!", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    // Send Selected Camera
    emit CameraSelected(QString::fromStdString(camera_name));
}

void CameraModule::OnBtnAddClicked()
{
    // Get Camera Info From Right Side
    std::string camera_name, address, host, port, username, password;
    CameraInfo(camera_name, address, host, port, username, password);
    if (camera_name.empty() || address.empty() || host.empty() || port.empty() || username.empty() || password.empty()) {
        QMessageBox::warning(NULL, "warning", "Please Complete Form of Right Side!", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    // get camera property
    int width, height, fps, total_frames;
    std::string camera_path = Utility::CameraAddress(username, password, host, port, address);
    if (!Utility::CameraProperty(camera_path, width, height, fps, total_frames)) {
        QMessageBox::warning(NULL, "warning", "Camera Unvalid!", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    // Check Camera Name in DB
    if (Utility::IsNameExistInDB("Cameras", "camera_name", camera_name)) {
        QMessageBox::warning(NULL, "warning", "Camera Exist!", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    // ADD Camera To DB
    if (!DBHelper::InsertToCameras(camera_name, width, height, address, host, port, username, password)) {
        QMessageBox::warning(NULL, "warning", "Add Camera Failed!", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    // Reload Camere Info
    LoadCameraInfo();
}

/* Process Delete Action */
void CameraModule::OnBtnDeleteClicked()
{
    if (p_tableview_->currentIndex().row() < 0)
        return;

    // Delete Camera
    if (!DeleteCameras(p_cameraname_->text().toStdString()))
        return;

    // Reload Camere Info
    LoadCameraInfo();

    // refresh tasktree
    emit CameraDeleted();
}

void CameraModule::OnTableViewPressed(const QModelIndex &index)
{
    std::map<std::string, std::string> mymap = p_camera_model_->RowElementsAt(index.row());

    p_cameraname_->setText(QString::fromStdString(mymap["camera_name"]));
    p_address_->setText(QString::fromStdString(mymap["address"]));
    p_host_->setText(QString::fromStdString(mymap["host"]));
    p_port_->setText(QString::fromStdString(mymap["port"]));
    p_username_->setText(QString::fromStdString(mymap["username"]));
    p_password_->setText(QString::fromStdString(mymap["password"]));
}

void CameraModule::CameraInfo(string &camera_name, string &address, string &host, string &port, string &username, string &password)
{
    camera_name = p_cameraname_->text().toStdString();
    address = p_address_->text().toStdString();
    host = p_host_->text().toStdString();
    port = p_port_->text().toStdString();
    username = p_username_->text().toStdString();
    password = p_password_->text().toStdString();
}

void CameraModule::LoadCameraInfo()
{
    std::vector<std::map<std::string, std::string> > res;
    if (DBHelper::SelectFromTable("Cameras", res)) {
        p_camera_model_->set_raw_data(res);
        p_tableview_->setModel(p_camera_model_);
    }
}

bool CameraModule::IsCameraUsing(string str)
{
    std::vector<std::map<std::string, std::string> > res;
    if (DBHelper::SelectFromTable("Tasks","task_name", str, res)) {
        if (res[0]["camera_status"] == kStatusON)
            return true;
    }

    return false;
}

bool CameraModule::DeleteCameras(const string &camera_name)
{
    // Search tasks that reference this camera
    std::vector<std::map<std::string, std::string> > res;
    if (!DBHelper::SelectFromTable("Task_Camera", "camera_name", camera_name, res))
        return false;

    for (size_t i = 0; i < res.size(); ++i) {
        // Check camera_status for each task that reference this camera
        if (IsCameraUsing(res[i]["task_name"])) {
            std::string str = "Camera " + camera_name + " is using by Task " + res[i]["task_name"] + ", Please Stop It First!";
            QMessageBox::warning(NULL, "warning", QString::fromStdString(str), QMessageBox::Ok, QMessageBox::Ok);
            return false;
        }
    }

    // Delete Camera and Tasks
    std::string str;
    if (res.empty()) {
        str = "Delete Camera " + camera_name + "?";
    } else {
        str = "Delete Camera " + camera_name + " will Delete follow tasks: \n";
        for (size_t i=0; i<res.size(); ++i) {
            str += res[i]["task_name"] + " ";
        }
    }

    QMessageBox::StandardButton btn_message = QMessageBox::warning(NULL, "warning", QString::fromStdString(str), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (btn_message == QMessageBox::No)
        return false;

    // Delete Camera
    if (! DBHelper::DeleteFromTable("Cameras", "camera_name", camera_name)) {
        QMessageBox::warning(NULL, "warning", "DELETE Camera FAILED!", QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }

    // Delete Tasks After Delete Camera
    for (size_t i=0; i<res.size(); ++i) {
        if (! DBHelper::DeleteFromTable("Tasks", "task_name", res[i]["task_name"])) {
            QMessageBox::warning(NULL, "warning", "Some Tasks DELETE FAILED!", QMessageBox::Ok, QMessageBox::Ok);
        }
    }

    return true;
}
