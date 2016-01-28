/****************************************************************************
 *
 *  Copyright (C) 2015 CUHK. All rights reserved.
 *  Author: Liping Zhang
 *
 * ****************************************************************************/

#ifndef UTILITY_H
#define UTILITY_H

#include <QObject>
#include <QtWidgets>
#include "dbhelper.h"

/**
 * @brief The Utility class
 */
class Utility : public QObject
{
    Q_OBJECT
public:
    static void MoveToCenter(QWidget *widget) {
        QSize size = widget->geometry().size();
        int width = qApp->desktop()->availableGeometry().size().width();
        int height = qApp->desktop()->availableGeometry().size().height();
        int mw = size.width();
        int mh = size.height();
        int centerW = (width/2) - (mw/2);
        int centerH = (height/2) - (mh/2);

        widget->move(centerW, centerH);
    }

    // check whether folder is exist or not
    static bool DirIsExist(QString strdir) {
        QDir dir (strdir);
        return dir.exists();
    }

    // check whether file is exist or not
    static bool FileIsExist(QString strfile) {
        QFile file(strfile);
        return file.exists();
    }
//    static bool FileIsExist(const std::string &strfile) {
//        if (boost::filesystem::exists(strfile)) {
//            return true;
//        }

//        return false;
//    }

    // get file name with it's extension
    static QString FileNameWithExtension(QString strfilepath) {
        QFileInfo fileInfo(strfilepath);
        return fileInfo.fileName();
    }

    // choose file;
    static QString GetFileName(QString filter) {
//        return QFileDialog::getOpenFileName(0, "FILE", QCoreApplication::applicationDirPath(), filter);
//        return QFileDialog::getOpenFileName(0, "FILE", QString("/home/lpzhang/ITF_DATA_CENTER/SHOWDATASET/videos"), filter);
        return QFileDialog::getOpenFileName(0, "FILE", QString::fromStdString(kHome), filter);
    }

    // check whether video or camera is still valid in tasks;
    static bool IsTaskValid(std::vector<std::map<string, string> > &res) {
        if (res[0]["camera_type"] == kCameraTypeFile) {
            std::vector<std::map<string, string> > res_files;
            std::string video_path;
            if (DBHelper::SelectFromTable("Files", "task_name", res[0]["task_name"], res_files) && !res_files.empty()) {
                video_path = res_files[0]["file_url"];
            } else {
                return false;
            }

            if (!IsCameraValid(video_path)) {
                QMessageBox::information(NULL, "Video Check", "Video file can not open", QMessageBox::Ok, QMessageBox::Ok);
                return false;
            }
        } else if (res[0]["camera_type"] == kCameraTypeHttp) {
            std::vector<std::map<string, string> > res_task_camera;
            std::string camera_path;
            if (DBHelper::SelectFromTable("Task_Camera", "task_name", res[0]["task_name"], res_task_camera) && !res_task_camera.empty()) {
                std::vector<std::map<string, string> > res_camera;
                if (DBHelper::SelectFromTable("Cameras", "camera_name", res_task_camera[0]["camera_name"], res_camera) && !res_camera.empty()) {
                    camera_path = CameraAddress(res_camera[0]["username"], res_camera[0]["password"], res_camera[0]["host"], res_camera[0]["port"], res_camera[0]["address"]);
                } else {
                    return false;
                }
            } else {
                return false;
            }

            if (!IsCameraValid(camera_path)) {
                QMessageBox::information(NULL, "Camera Check", "Camera can not open", QMessageBox::Ok, QMessageBox::Ok);
                return false;
            }
        } else {
            std::cout << "camera type error" << std::endl;
            return false;
        }

        return true;
    }

    /* get Camera Address*/
    static std::string CameraAddress(const std::string &username, const std::string &password, const std::string &host, const std::string &port, const std::string &suffix) {
        return std::string("http://" + username + ":" + password + "@" + host + ":" + port + "/" + suffix);
    }

    /*Test Video/Camera is valid or not*/
    static bool IsCameraValid(const std::string &str) {
        cv::VideoCapture cap(str);
        if (!cap.isOpened())
            return false;
        return true;
    }

    /*Get information of Video/Camera*/
    static bool CameraProperty(const std::string &str, int &width, int &height, int &fps, int &total_frames) {
        cv::VideoCapture cap(str);
        if (!cap.isOpened())
            return false;

        width = static_cast<int> (cap.get(CV_CAP_PROP_FRAME_WIDTH));
        height = static_cast<int> (cap.get(CV_CAP_PROP_FRAME_HEIGHT));
        fps = static_cast<int> (cap.get(CV_CAP_PROP_FPS));
        total_frames = static_cast<int> (cap.get(CV_CAP_PROP_FRAME_COUNT));

        return true;
    }

    /**
     * @brief IsNameExistInDB
     * @param table
     * @param key
     * @param val
     * @return
     */
    static bool IsNameExistInDB(const string &table, const string &key, const string &name) {
        // Check task_name in DB
        std::vector<std::map<std::string, std::string> > res;
        if (DBHelper::SelectFromTable(table, res)) {
            for (size_t i = 0; i < res.size(); ++i) {
                if (name == res[i][key]) {
                    return true;
                }
            }
        }
        return false;
    }

    // Create File Folder;
    static bool CreateFileFolder(const std::string str) {
        if (!boost::filesystem::is_directory(str)) {
            if (!boost::filesystem::create_directories(str)) {
                std::cout << "Create Folder " << str << " Success!" << std::endl;
                return true;
            } else {
                std::cout << "Create Folder " << str << " Failed!" << std::endl;
                return false;
            }
        }

        return true;
    }

    // Delete File
    static bool RemoveFile(const std::string str) {
        if (boost::filesystem::exists(str)) {
            if (!boost::filesystem::remove(str)) {
                return false;
            }
        }

        return true;
    }

    // Delete File Folder
    static bool RemoveFileFolder(const std::string str) {
        if (boost::filesystem::is_directory(str)) {
            if (boost::filesystem::remove_all(str)) {
                std::cout << "Remove Folder " << str << " Success!" << std::endl;
                return true;
            } else {
                std::cout << "Remove Folder " << str << " Failed!" << std::endl;
                return false;
            }
        }

        return true;
    }

    static bool SavePoints(QVector<QPointF> points, const std::string &file_path) {
        std::ofstream myfile(file_path);

        if (myfile.is_open()) {
            for (int i = 0; i < points.size(); ++i) {
                float x = points[i].x();
                float y = points[i].y();
                myfile << x << "," << y << std::endl;
            }

            myfile.close();
            return true;
        } else {
            std::cout << "Unable to open file" << std::endl;
            return false;
        }
    }

    template <typename T>
    static bool LoadPoints(QVector<T> &points, const std::string &file_path) {
        std::ifstream myfile(file_path);

        if (myfile.is_open()) {
            string value;
            while (myfile.good()) {
                getline(myfile, value, ',');
                float x = atof(value.c_str());
                getline(myfile, value, '\n');
                float y = atof(value.c_str());
                points.append(T(x, y));
            }

            myfile.close();

            // Last One is (0, 0), Remove
            points.pop_back();

            return true;
        } else {
            std::cout << "Unable to open file" << std::endl;
            return false;
        }
    }

    static bool ReadGTData(const string &dir, QVector<QVector<QPointF> > &gt_points, QVector<cv::Mat> &gt_images) {
        if (!boost::filesystem::is_directory(dir)) {
                cout << dir << " don't exist" << endl;
                return false;
        }
        boost::filesystem::directory_iterator end_iter;
        for (boost::filesystem::directory_iterator file_iter(dir); file_iter != end_iter; ++file_iter) {
            if (!boost::filesystem::is_directory(*file_iter) && (boost::filesystem::extension(*file_iter) == kGTImageSuffix)) {
                boost::filesystem::path p = file_iter->path();
                std::string gt_img_file = p.string();
                std::string gt_point_file = p.replace_extension(kGTAnnotationSuffix).string();

                // Load GT Points for Sketchpad
                QVector<QPointF> gt_point;
                if (! LoadPoints(gt_point, gt_point_file)) {
                    return false;
                }
                gt_points.append(gt_point);

                // Load Image
                cv::Mat img = cv::imread(gt_img_file);
                gt_images.append(img);
            }
        }

        return true;
    }

    static bool WriteDGData(const string &dir, const QVector<QVector<QPointF> > &gt_points, const QVector<cv::Mat> &gt_images) {
        /* Remove GT First */
        if (!RemoveFileFolder(dir)) {
            return false;
        }
        /* Create GT Folder*/
        if (!CreateFileFolder(dir)) {
            return false;
        }
        for (int i=0; i<gt_images.size(); ++i) {
            std::string gt_image_path = dir + std::to_string(i) + kGTImageSuffix;
            std::string gt_annotation_path = dir + std::to_string(i) + kGTAnnotationSuffix;
            /* save gt_points*/
            if (!SavePoints(gt_points[i], gt_annotation_path)) {
                return false;
            }
            /* save images */
            cv::imwrite(gt_image_path, gt_images[i]);
        }
        return true;
    }

    static void SetWdgSizePolicy(QWidget *p_w, QSizePolicy::Policy horizontal, QSizePolicy::Policy vertical) {
        QSizePolicy sizepolicy(horizontal, vertical);
        sizepolicy.setHorizontalStretch(0);
        sizepolicy.setVerticalStretch(0);
        sizepolicy.setHeightForWidth(p_w->sizePolicy().hasHeightForWidth());
        p_w->setSizePolicy(sizepolicy);
        p_w->setFocusPolicy(Qt::StrongFocus);
    }

    static void SetLabelStyle(QLabel *lab) {
        SetWdgSizePolicy(lab, QSizePolicy::Ignored, QSizePolicy::Ignored);

        lab->setFrameShape(QFrame::Panel);
        lab->setFrameShadow(QFrame::Plain);
//        lab->setLineWidth(1);
//        lab->setMidLineWidth(0);
        lab->setAlignment(Qt::AlignCenter);
//        lab->setScaledContents(true);

        lab->setBackgroundRole(QPalette::Base);
    }
};

#endif // UTILITY_H
