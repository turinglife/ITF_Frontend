#include "cameramanager.h"
#include "ucameramanager.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QHeaderView>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QTableView>
#include <QLineEdit>
#include <QDebug>
#include "utility.h"
#include "dbi.hpp"
#include "tablemodel.h"


CameraManager::CameraManager(QWidget *parent) : QDialog(parent)
{
    Init();
}

CameraManager::~CameraManager()
{
//    delete ui_camera_;
}

void CameraManager::Init()
{
    this->setProperty("Form", true);
    this->setGeometry(qApp->desktop()->availableGeometry());

    QVBoxLayout *layout = new QVBoxLayout;
    ui_camera_ = new UCameraManager(this);

    ui_camera_->tableview_->horizontalHeader()->setStretchLastSection(true);
    ui_camera_->tableview_->setAlternatingRowColors(true);
    ui_camera_->tableview_->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui_camera_->tableview_->setSelectionMode(QAbstractItemView::SingleSelection);

    layout->addWidget(ui_camera_);
    this->setLayout(layout);

    // Setup Connection
    connect(ui_camera_->tableview_, SIGNAL(clicked(QModelIndex)), this, SLOT(on_tableView_pressed(QModelIndex)));
    connect(ui_camera_->btn_select_, SIGNAL(clicked()), this, SLOT(on_btnSelect_clicked()));
    connect(ui_camera_->btn_add_, SIGNAL(clicked()), this, SLOT(on_btnAdd_clicked()));
    connect(ui_camera_->btn_delete_, SIGNAL(clicked()), this, SLOT(on_btnDelete_clicked()));
    connect(ui_camera_->btn_menu_close_, SIGNAL(clicked()), this, SLOT(close()));

    // Setup model and Load Camera Info
    camera_model_ = new TableModel(this);
    LoadCameraInfo();
}

bool CameraManager::GetCameraInfo(std::string &camera_name, std::string &address, std::string &host, std::string &port, std::string &username, std::string &password)
{
    camera_name = ui_camera_->cameraname_->text().toStdString();
    address = ui_camera_->address_->text().toStdString();
    host = ui_camera_->host_->text().toStdString();
    port = ui_camera_->port_->text().toStdString();
    username = ui_camera_->username_->text().toStdString();
    password = ui_camera_->password_->text().toStdString();

    if (camera_name.empty() || address.empty() || host.empty() || port.empty() || username.empty() || password.empty()) {
        QMessageBox::warning(NULL, "warning", "Please Complete Form of Right Side!", QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }

    return true;
}

/* Process Select Action */
void CameraManager::on_btnSelect_clicked()
{
    // Get Camera Info From Right Side
    int width, height, fps, total_frames;
    std::string camera_name, address, host, port, username, password;
    if (!GetCameraInfo(camera_name, address, host, port, username, password)) {
        return;
    }

    // Check Camera is Valid or Not
    std::string camera_path = "http://" + username + ":" + password + "@" + host + ":" + port + "/" + address;
    if (! Utility::IsCameraValid(camera_path, width, height, fps, total_frames)) {
        QMessageBox::warning(NULL, "warning", "Camera Unvalid!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return;
    }

    // Send Selected Camera
    emit SelectedCamera(QString::fromStdString(camera_name));
}

/* Process ADD Action */
void CameraManager::on_btnAdd_clicked()
{
    // Get Camera Info From Right Side
    std::string camera_name, address, host, port, username, password;
    if (!GetCameraInfo(camera_name, address, host, port, username, password)) {
        return;
    }

    // Check Camera Name in DB
    if (IsExistCameraName(camera_name)) {
        QMessageBox::warning(NULL, "warning", "Camera Name Exist!", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    // ADD Camera To DB
    if (!InsertToCameras(camera_name, address, host, port, username, password)) {
        return;
    }

    // Reload Camere Info
    LoadCameraInfo();
}

/* Process Delete Action */
void CameraManager::on_btnDelete_clicked()
{
    if (ui_camera_->tableview_->currentIndex().row() < 0) {
        return;
    }

    // Delete Camera
    if (!DeleteFromCameras(ui_camera_->cameraname_->text().toStdString())) {
        return;
    }

    // Reload Camere Info
    LoadCameraInfo();

    // refresh tasktree
}

/* Load All Cameras Info */
void CameraManager::LoadCameraInfo()
{
    ui_camera_->Clear();

    // Connect to DB
    CDbi db_;
    if (! Utility::IsDBConnected(db_)) {
        QMessageBox::warning(0, "Warning", "Connect DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    std::vector<std::map<std::string, std::string> > res = db_.Query("SELECT camera_name, address, host, port, username, password FROM Cameras;");

    camera_model_->set_raw_data(res);

    ui_camera_->tableview_->setModel(camera_model_);
}

/* Check camera_name in Cameras of DB */
bool CameraManager::IsExistCameraName(std::string str)
{
    // Connect to DB
    CDbi db_;
    if (! Utility::IsDBConnected(db_)) {
        QMessageBox::warning(0, "Warning", "Connect DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }
    std::vector<std::map<std::string, std::string> > res = db_.Query("SELECT camera_name FROM Cameras;");

    for (size_t i = 0; i < res.size(); ++i) {
            std::cout << res[i]["camera_name"] << std::endl;
            if (str == res[i]["camera_name"]) {
                return true;
            }
    }
    return false;
}

/* Check Camera Status */
bool CameraManager::IsCameraUsing(std::string str)
{
    // Connect to DB
    CDbi db_;
    if (! Utility::IsDBConnected(db_)) {
        QMessageBox::warning(0, "Warning", "Connect DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }
    std::vector<std::map<std::string, std::string> > res = db_.Query("SELECT camera_status FROM Tasks WHERE task_name='" + str + "';");

    if (res[0]["camera_status"] == kStatusON) {
        return true;
    }

    return false;
}

bool CameraManager::InsertToCameras(std::string camera_name, std::string address, std::string host, std::string port, std::string username, std::string password)
{
    // Check Camera is Valid or Not
    int width, height, fps, total_frames;
    std::string camera_path = "http://" + username + ":" + password + "@" + host + ":" + port + "/" + address;
    if (! Utility::IsCameraValid(camera_path, width, height, fps, total_frames)) {
        QMessageBox::warning(NULL, "warning", "Camera Unvalid!", QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }

    // Connect to DB
    CDbi db_;
    if (! Utility::IsDBConnected(db_)) {
        QMessageBox::warning(0, "Warning", "Connect DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }
    // Insert New Camera info to DB
    bool ok = db_.RunSQL("INSERT INTO Cameras VALUES ('"
                         + camera_name + "', "
                         + std::to_string(width) + ", "
                         + std::to_string(height) + ", '"
                         + address + "', '"
                         + host + "', '"
                         + port + "', '"
                         + username + "', '"
                         + password + "');");

    if (!ok) {
        QMessageBox::warning(NULL, "warning", "Camera Add Failed!", QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }

    return true;
}

bool CameraManager::DeleteFromCameras(string camera_name)
{
    // Connect to DB
    CDbi db_;
    if (! Utility::IsDBConnected(db_)) {
        QMessageBox::warning(0, "Warning", "Connect DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }
    // Search tasks that reference this camera
    std::vector<std::map<std::string, std::string> > res = db_.Query("SELECT task_name FROM Task_Camera WHERE camera_name='" + camera_name + "';");

    for (size_t i = 0; i < res.size(); ++i) {
        // Check camera_status for each task that reference this camera
        if (IsCameraUsing(res[i]["task_name"])) {
            std::string str = "Camera " + camera_name + " is using by Task " + res[i]["task_name"] + ", Please Stop It First!";
            QMessageBox::warning(NULL, "warning", QString::fromStdString(str), QMessageBox::Ok, QMessageBox::Ok);
            return false;
        }
    }

    // Delete Camera and Tasks
    std::string str = "Delete Camera " + camera_name + " will Delete follow tasks: ";
    for (size_t i=0; i<res.size(); ++i) {
        str += res[i]["task_name"] + " ";
    }

    QMessageBox::StandardButton btn_message = QMessageBox::warning(NULL, "warning", QString::fromStdString(str), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (btn_message == QMessageBox::Yes) {
        // Delete Camera
        bool ok = db_.RunSQL("DELETE FROM Cameras WHERE camera_name='" + camera_name + "';");
        if (!ok) {
            QMessageBox::warning(NULL, "warning", "DELETE DB FAILED!", QMessageBox::Ok, QMessageBox::Ok);
            return false;
        }

        // Delete Tasks After Delete Camera
        for (size_t i=0; i<res.size(); ++i) {
            db_.RunSQL("DELETE FROM Tasks WHERE task_name='" + res[i]["task_name"] + "';");
        }
    }

    return true;
}

void CameraManager::on_tableView_pressed(const QModelIndex &index)
{
    std::map<std::string, std::string> mymap = camera_model_->RowElementsAt(index.row());

    ui_camera_->cameraname_->setText(QString::fromStdString(mymap["camera_name"]));
    ui_camera_->address_->setText(QString::fromStdString(mymap["address"]));
    ui_camera_->host_->setText(QString::fromStdString(mymap["host"]));
    ui_camera_->port_->setText(QString::fromStdString(mymap["port"]));
    ui_camera_->username_->setText(QString::fromStdString(mymap["username"]));
    ui_camera_->password_->setText(QString::fromStdString(mymap["password"]));
}
