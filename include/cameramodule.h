#ifndef CAMERAMODULE_H
#define CAMERAMODULE_H

#include <QWidget>
#include "common.h"

class QTableView;
class QLineEdit;
class TableModel;
class QModelIndex;

class CameraModule : public QWidget
{
    Q_OBJECT
public:
    explicit CameraModule(bool btn_select, bool btn_add, bool btn_delete, QWidget *parent = 0);
    ~CameraModule();

signals:
    void CameraSelected(QString);
    void CameraDeleted();

private slots:
    void OnBtnSelectClicked();
    void OnBtnAddClicked();
    void OnBtnDeleteClicked();
    void OnTableViewPressed(const QModelIndex &index);

private:
    bool CameraInfo(std::string &camera_name, std::string &address, std::string &host, std::string &port, std::string &username, std::string &password, int &width, int &height, int &fps, int &total_frames);
    void LoadCameraInfo();
    bool IsCameraUsing(string str);
    bool DeleteCameras(const string &camera_name);

    TableModel *p_camera_model_;
    QTableView *p_tableview_;
    QLineEdit *p_cameraname_;
    QLineEdit *p_address_;
    QLineEdit *p_host_;
    QLineEdit *p_port_;
    QLineEdit *p_username_;
    QLineEdit *p_password_;
};

#endif // CAMERAMODULE_H
