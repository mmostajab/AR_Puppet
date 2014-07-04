TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lGL -lGLU -lopencv_core -lopencv_highgui -lopencv_imgproc -lglfw

SOURCES += \
    Exercise-9.cpp \
    MarkerTracker.cpp \
    PoseEstimation.cpp

HEADERS += \
    MarkerTracker.h \
    PoseEstimation.h \
    myGL.h

