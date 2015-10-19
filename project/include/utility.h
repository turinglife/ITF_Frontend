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
#include "common.h"
#include "dbi.hpp"

/**
 * @brief The Utility class
 */
class Utility : public QObject
{
    Q_OBJECT
public:
    static void MoveToCenter(QWidget *widget) {
        QSize size = widget->geometry().size();
//        QSize size = widget->sizeHint();
        int width = qApp->desktop()->availableGeometry().size().width();
        int height = qApp->desktop()->availableGeometry().size().height();
        int mw = size.width();
        int mh = size.height();
        int centerW = (width/2) - (mw/2);
        int centerH = (height/2) - (mh/2);

        qDebug() << width << " " << height;
        qDebug() << mw << " " << mh;

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
        return QFileDialog::getOpenFileName(0, "FILE", QString("/home/lpzhang/ITF_DATA_CENTER/SHOWDATASET/videos"), filter);
    }

    // check whether video or camera is valid;
    static bool IsCameraValid(const std::string str, int &width, int &height, int &fps, int &total_frames) {
        cv::VideoCapture cap(str);
        if (!cap.isOpened()) {
            return false;
        }

        width = static_cast<int> (cap.get(CV_CAP_PROP_FRAME_WIDTH));
        height = static_cast<int> (cap.get(CV_CAP_PROP_FRAME_HEIGHT));
        fps = static_cast<int> (cap.get(CV_CAP_PROP_FPS));
        total_frames = static_cast<int> (cap.get(CV_CAP_PROP_FRAME_COUNT));

        return true;
    }

    // connect to database;
    static bool IsDBConnected(CDbi &db) {
        std::cout << "Connect ... " << std::endl;
        if (!db.Connect(server, user, pass)) {
            std::cout << "Fail" << std::endl;
            return false;
        } else {
            std::cout << "OK" << std::endl;
        }

        std::cout<< "Select DB ... " << std::endl;
        if (!db.UseDB(db_name)) {
            std::cout << "Fail" << std::endl;
            return false;
        } else {
            std::cout << "OK" << std::endl;
        }

        return true;
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

    static bool SavePoints(QVector<QPointF> points, const std::string &file_path, float rx, float ry) {
//        std::cout << "Path: " << file_path << std::endl;
//        std::cout << "rx, ry: " << rx << ", " << ry<< std::endl;
        std::ofstream myfile(file_path);

        if (myfile.is_open()) {
            for (int i = 0; i < points.size(); ++i) {
                float x = points[i].x() * rx;
                float y = points[i].y() * ry;
//                float x = points[i].x();
//                float y = points[i].y();
//                std::cout << "orig: " << x << ", " << y << std::endl;
//                x *= rx;
//                y *= ry;
//                std::cout << "---: " << x << ", " << y << std::endl;

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
    static bool LoadPoints(QVector<T> &points, const std::string &file_path, float rx, float ry) {
//        std::cout << "Path: " << file_path << std::endl;
//        std::cout << "rx, ry: " << rx << ", " << ry<< std::endl;
        std::ifstream myfile(file_path);

        if (myfile.is_open()) {
            string value;
            while (myfile.good()) {
                getline(myfile, value, ',');
                float x = atof(value.c_str());
                getline(myfile, value, '\n');
                float y = atof(value.c_str());
//                std::cout << "orig: " << x << ", " << y << std::endl;
                x *= rx;
                y *= ry;
//                std::cout << "---: " << x << ", " << y << std::endl;

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

    static void SetLabelStyle(QLabel *lab) {
        QSizePolicy sizepolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        sizepolicy.setHorizontalStretch(0);
        sizepolicy.setVerticalStretch(0);
        sizepolicy.setHeightForWidth(lab->sizePolicy().hasHeightForWidth());
        lab->setSizePolicy(sizepolicy);
        lab->setFocusPolicy(Qt::StrongFocus);
        lab->setFrameShape(QFrame::Panel);
        lab->setFrameShadow(QFrame::Plain);
        lab->setLineWidth(1);
        lab->setMidLineWidth(0);
        lab->setAlignment(Qt::AlignCenter);
        lab->setScaledContents(true);

        lab->setBackgroundRole(QPalette::Base);
    }
};

#endif // UTILITY_H
