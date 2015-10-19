/****************************************************************************
 *
 *  Copyright (C) 2015 CUHK. All rights reserved.
 *  Author: Liping Zhang
 *
 * ****************************************************************************/
#ifndef COMMON_H
#define COMMON_H

// standards system header file
#include <fcntl.h>
#include <stdint.h>
#include <fstream>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdarg.h>

// linux system header file
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>

//
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#define IN        // Input parameters
#define OUT       // Output parameters

using namespace std;

const string kAppDir = "ITF_SmartClient/";
const string kTaskDir = "Task/";
const string kPersDir = "PMap/";
const string kROIDir = "ROI/";
const string kLMDir = "LM/";
const string kGTDir = "GT/";
const string kAlarmDir = "Alarm/";

const string kPersFile = "pers.csv";
const string kROIFile = "roi.csv";
const string kLMFile = "lm.csv";

const string kHome = string(getenv("HOME")) + "/Desktop";
const string kAppRootPath = kHome + "/" + kAppDir;
const string kTaskRootPath = kAppRootPath + kTaskDir;

const string kGTAnnotationSuffix = ".csv";
const string kGTImageSuffix = ".jpg";

/* Prepare DB Infomation*/
const string server = "localhost";
const string user = "itf";
const string pass = "itf";
const string db_name = "ITF";

/* Daemon */
const string kDaemonAD = "AD";
const string kDaemonCD = "CD";
const string kDaemonRD = "RD";

/* Command */
const string kCommandStart = "START";
const string kCommandStop = "STOP";

/*Receive message */
const string kMessageReceivedOk = "OK";
const string kMessageReceivedError = "ERROR";

/* task type*/
const string kTaskTypeCounting = "COUNTING";
const string kTaskTypeSegmentation = "SEGMENTATION";
const string kTaskTypeStationary = "STATIONARY";

/* Camera type */
const string kCameraTypeFile = "FILE";
const string kCameraTypeHttp = "HTTP";
/* status */
const string kStatusON = "ON";
const string kStatusOFF= "OFF";

/* Priority */
const string kPriorityLow = "LOW";
const string kPriorityMedium = "MEDIUM";
const string kPriorityHigh = "HIGH";


enum DrawType {
    ROI = 0,
    PersMap = 1,
    GTPoints = 2
};

/*countingsetting*/
const int kSketchpadWidth = 640;
const int kSketchpadHeight = 360;
const int kGroupWidth = 240;
const int kGroupHeight = 120;
const int kTaskWidth = 640;
const int kTaskHeight = 270;


#endif // COMMON_H
