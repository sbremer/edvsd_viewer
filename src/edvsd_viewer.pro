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
    edvsd_anormaly_detection.cpp \
	neuralnet/neuralnet.cpp \
    neuralnet/neuronlayer.cpp \
    neuralnet/neuron.cpp \
	kohonentracking/kohonentracking.cpp \
    output/gnuplot_output.cpp \
    neuralnet_driver/neuralnet_driver.cpp \
    geneticalgorithm/geneticalgoritm.cpp \
    startendtracker/startendtracker.cpp

HEADERS  += edvsd_viewer.h \
    edvsd.h \
    edvsd_visualizer.h \
    edvsd_fileprocessor.h \
    edvsd_anormaly_detection.h \
	neuralnet/neuralnet.h \
    neuralnet/neuronlayer.h \
    neuralnet/neuron.h \
    neuralnet/neuralnet_functions.h \
	kohonentracking/kohonentracking.h \
	kohonentracking/kohonentracking_helper.h \
	helper/pointf.h \
	kohonentracking/kohonenmap.h \
    output/gnuplot_output.h \
    neuralnet_driver/neuralnet_driver.h \
    geneticalgorithm/geneticalgoritm.h \
    startendtracker/startendtracker.h \
    helper/motionf.h

FORMS    += edvsd_viewer.ui
