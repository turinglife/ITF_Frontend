#-------------------------------------------------
#
# Project created by QtCreator 2015-07-13T11:07:50
#
#-------------------------------------------------

QT       += core gui \
            sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = ITF_SmartClient
TEMPLATE = app

CONFIG += c++11

INCLUDEPATH += ./include \
               ./include/ui

SOURCES += ./src/main.cpp\
        ./src/mainwindow.cpp \
    ./src/qcustomplot.cpp \
    ./src/AppConfig.cpp \
    ./src/newtask.cpp \
    ./src/delegate.cpp \
    ./src/cameramanager.cpp \
    ./src/taskcommandcenter.cpp \
    ./src/sketchpad.cpp \
    ./src/ui/utaskname.cpp \
    ./src/ui/utasktype.cpp \
    ./src/ui/utaskcamera.cpp \
    ./src/ui/utaskalarm.cpp \
    ./src/ui/utaskreport.cpp \
    ./src/ui/utaskbaseinfo.cpp \
    ./src/ui/utaskboard.cpp \
    ./src/ui/utreewidget.cpp \
    ./src/ui/uclipwidget.cpp \
    ./src/ui/ucameramanager.cpp \
    ./src/ui/ucountingsetting.cpp \
    ./src/ui/unewgroup.cpp \
    ./src/ui/uanalyzeboard.cpp \
    ./src/ui/uplotwidget.cpp \
    ./src/alarmfiltermodel.cpp \
    ./src/tablemodel.cpp \
    ./src/alarmanalyzer.cpp \
    ./src/lightwidget.cpp \
    ./src/alarmwidget.cpp

HEADERS  += ./include/mainwindow.h \
    ./include/qcustomplot.h \
    ./include/common.h \
    ./include/utility.h \
    ./include/dbi.hpp \
    ./include/buffer.hpp \
    ./include/AppConfig.h \
    ./include/newtask.h \
    ./include/delegate.h \
    ./include/cameramanager.h \
    ./include/taskcommandcenter.h \
    ./include/sketchpad.h \
    ./include/ui/utaskname.h \
    ./include/ui/utasktype.h \
    ./include/ui/utaskcamera.h \
    ./include/ui/utaskalarm.h \
    ./include/ui/utaskreport.h \
    ./include/ui/utaskbaseinfo.h \
    ./include/ui/utaskboard.h \
    ./include/ui/utreewidget.h \
    ./include/ui/uclipwidget.h \
    ./include/ui/ucameramanager.h \
    ./include/ui/ucountingsetting.h \
    ./include/ui/unewgroup.h \
    ./include/ui/uanalyzeboard.h \
    ./include/ui/uplotwidget.h \
    ./include/alarmfiltermodel.h \
    ./include/tablemodel.h \
    ./include/alarmanalyzer.h \
    ./include/lightwidget.h \
    ./include/alarmwidget.h

unix{
#INCLUDEPATH += /usr/local/include/itf

LIBS += -L/usr/local/lib \
        -litfbackend
#        -litf -litfbackend

LIBS += -L/usr/local/lib \
        -lopencv_core \
        -lopencv_imgproc \
        -lopencv_highgui \
        -lboost_system \
        -lboost_filesystem \

LIBS += -L/usr/lib/x86_64-linux-gnu \
        -lrt


LIBS += -L/usr/local/cuda/lib64 \
        -lcublas \
        -lcurand \
        -lcudnn

}
