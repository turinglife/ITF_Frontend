/****************************************************************************
 *
 *  Copyright (C) 2015 CUHK. All rights reserved.
 *  Author: Liping Zhang
 *
 * ****************************************************************************/
#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include <QDialog>
#include "common.h"

class UCameraManager;
class TableModel;
class QSqlQueryModel;
class QModelIndex;

/**
 * @brief The CameraManager class is system to manage camera, includes
 * select, add and delete cameras
 */
class CameraManager : public QDialog
{
    Q_OBJECT
public:
    explicit CameraManager(QWidget *parent = 0);
    ~CameraManager();

signals:
    void SelectedCamera(QString);

private slots:
    void on_btnSelect_clicked();
    void on_btnAdd_clicked();
    void on_btnDelete_clicked();
    void on_tableView_pressed(const QModelIndex &index);

private:
    void Init();
    bool GetCameraInfo(std::string &camera_name, std::string &address, std::string &host, std::string &port, std::string &username, std::string &password);

    /**
     * @brief LoadCameraInfo	Load all cameras info from database;
     */
    void LoadCameraInfo();

    /**
     * @brief IsExistCameraName	check camera name in database
     * @param str 	camera name
     * @return 	true if camera name already exist in database
     */
    bool IsExistCameraName(string str);

    bool IsCameraUsing(string str);

    bool InsertToCameras(string camera_name, string address, string host, string port, string username, string password);
    bool DeleteFromCameras(std::string camera_name);


    /**
     * @brief ui_camera_ 	ui for camera manage system
     */
    UCameraManager *ui_camera_;
    TableModel *camera_model_;
};

#endif // CAMERAMANAGER_H
