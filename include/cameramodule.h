#ifndef CAMERAMODULE_H
#define CAMERAMODULE_H

#include <QWidget>
#include "common.h"
#include <QPushButton>

class QTableView;
class QLineEdit;
class TableModel;
class QModelIndex;

class CameraModule : public QWidget
{
    Q_OBJECT
public:
    explicit CameraModule(QWidget *parent = 0);
    ~CameraModule();

    void set_flag(bool f_select, bool f_add, bool f_delete, bool f_lineedit);

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

    QPushButton *p_select_;
    QPushButton *p_add_;
    QPushButton *p_delete_;
};

#endif // CAMERAMODULE_H
