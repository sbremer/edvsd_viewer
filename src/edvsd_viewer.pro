#-------------------------------------------------
#
# Project created by QtCreator 2013-08-19T02:59:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = edvsd_viewer
TEMPLATE = app


SOURCES += main.cpp\
        edvsd_viewer.cpp \
    edvsd_visualizer.cpp \
    edvsd_fileprocessor.cpp \
    edvsd_anormaly_detection.cpp

HEADERS  += edvsd_viewer.h \
    edvsd.h \
    edvsd_visualizer.h \
    edvsd_fileprocessor.h \
    edvsd_anormaly_detection.h

FORMS    += edvsd_viewer.ui
