#-------------------------------------------------
#
# Project created by QtCreator 2013-05-16T20:15:31
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = CameraCalibration
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += /opt/local/include

LIBS += -L/usr/local/lib \
        -lopencv_calib3d \
        -lopencv_contrib \
        -lopencv_features2d \
        -lopencv_flann \
        -lopencv_gpu \
        -lopencv_imgproc \
        -lopencv_ml \
        -lopencv_objdetect \
        -lopencv_video \
        -lopencv_highgui \
        -lopencv_core

SOURCES += main.cpp
