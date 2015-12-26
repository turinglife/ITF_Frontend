#-------------------------------------------------
#
# Project created by QtCreator 2015-12-09T16:25:23
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = IntelligentVideoAnalysis
TEMPLATE = app

CONFIG += c++11

INCLUDEPATH += ./include

SOURCES += ./src/main.cpp\
    ./src/utbasewidget.cpp \
    ./src/utname.cpp \
    ./src/uttype.cpp \
    ./src/utcamera.cpp \
    ./src/utalarmcounting.cpp \
    ./src/utalarmstationary.cpp \
    ./src/tablemodel.cpp \
    ./src/groupwidget.cpp \
    ./src/taskwidget.cpp \
    ./src/tasktreewidget.cpp \
    ./src/qcustomplot.cpp \
    ./src/uclipglwidget.cpp \
    ./src/uclipwidget.cpp \
    ./src/uclipboard.cpp \
    ./src/uclipcontroler.cpp \
    ./src/intelligentvideoanalysis.cpp \
    ./src/taskcommandcenter.cpp \
    ./src/alarmfiltermodel.cpp \
    ./src/uplotwidget.cpp \
    ./src/sketchpad.cpp \
    ./src/delegate.cpp \
    ./src/usketchpadwidget.cpp \
    ./src/countingsetting.cpp \
    ./src/utalarmstrategy.cpp \
    ./src/cameramodule.cpp \
    ./src/alarmmodule.cpp \
    ./src/utasktree.cpp \
    ./src/analysismodule.cpp \
    ./src/uclipanalysisboard.cpp \
    ./src/ulightwidget.cpp \
    ./src/uclipalarmwidget.cpp

HEADERS  += \
    ./include/utbasewidget.h \
    ./include/utname.h \
    ./include/uttype.h \
    ./include/utcamera.h \
    ./include/utalarmcounting.h \
    ./include/utalarmstationary.h \
    ./include/dbi.hpp \
    ./include/buffer.hpp \
    ./include/utility.h \
    ./include/common.h \
    ./include/tablemodel.h \
    ./include/dbhelper.h \
    ./include/groupwidget.h \
    ./include/taskwidget.h \
    ./include/tasktreewidget.h \
    ./include/qcustomplot.h \
    ./include/uclipglwidget.h \
    ./include/uclipwidget.h \
    ./include/uclipboard.h \
    ./include/uclipcontroler.h \
    ./include/intelligentvideoanalysis.h \
    ./include/taskcommandcenter.h \
    ./include/alarmfiltermodel.h \
    ./include/uplotwidget.h \
    ./include/sketchpad.h \
    ./include/delegate.h \
    ./include/usketchpadwidget.h \
    ./include/countingsetting.h \
    ./include/utalarmstrategy.h \
    ./include/cameramodule.h \
    ./include/alarmmodule.h \
    ./include/utasktree.h \
    ./include/analysismodule.h \
    ./include/uclipanalysisboard.h \
    ./include/ulightwidget.h \
    ./include/uclipalarmwidget.h


unix{
LIBS += -L/usr/local/lib \
        -litfbackend

LIBS += -L/usr/local/lib \
        -lopencv_core \
        -lopencv_imgproc \
        -lopencv_highgui \
        -lboost_system \
        -lboost_filesystem \

LIBS += -L/usr/lib/x86_64-linux-gnu \
        -lrt
}
