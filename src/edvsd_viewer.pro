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
    archive/neuralnet/neuralnet.cpp \
    archive/neuralnet/neuronlayer.cpp \
    archive/neuralnet/neuron.cpp \
    archive/kohonentracking/kohonentracking.cpp \
    output/gnuplot_output.cpp \
    archive/neuralnet_driver/neuralnet_driver.cpp \
    archive/startendtracker/startendtracker.cpp \
    archive/edvsd_analyzer.cpp \
    archive/genetricalgorithm_driver/geneticalgorithm_driver.cpp \
    archive/geneticalgorithm/geneticalgorithm.cpp \
    growingneuralgas/growingneuralgas.cpp \
    growingneuralgar_driver/growingneuralgas_driver.cpp \
    helper/random.cpp \
    dyntracker/dyntracker.cpp

HEADERS  += edvsd_viewer.h \
    edvsd.h \
    edvsd_visualizer.h \
    edvsd_fileprocessor.h \
    edvsd_anormaly_detection.h \
    archive/neuralnet/neuralnet.h \
    archive/neuralnet/neuronlayer.h \
    archive/neuralnet/neuron.h \
    archive/neuralnet/neuralnet_functions.h \
    archive/kohonentracking/kohonentracking.h \
    archive/kohonentracking/kohonentracking_helper.h \
    helper/pointf.h \
    archive/kohonentracking/kohonenmap.h \
    output/gnuplot_output.h \
    archive/neuralnet_driver/neuralnet_driver.h \
    archive/startendtracker/startendtracker.h \
    helper/motionf.h \
    archive/edvsd_analyzer.h \
    helper/eventf.h \
    archive/genetricalgorithm_driver/geneticalgorithm_driver.h \
    archive/geneticalgorithm/geneticalgorithm.h \
    growingneuralgas/growingneuralgas.h \
    growingneuralgar_driver/growingneuralgas_driver.h \
    helper/random.h \
    dyntracker/dyntracker.h \
    helper/feature_event.h

FORMS    += edvsd_viewer.ui
