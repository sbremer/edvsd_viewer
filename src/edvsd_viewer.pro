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
    startendtracker/startendtracker.cpp \
    edvsd_analyzer.cpp \
    genetricalgorithm_driver/geneticalgorithm_driver.cpp \
    geneticalgorithm/geneticalgorithm.cpp \
	growingneuralgas/growingneuralgas.cpp \
    growingneuralgar_driver/growingneuralgas_driver.cpp \
	helper/random.cpp \
    dyntracker/dyntracker.cpp

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
    startendtracker/startendtracker.h \
    helper/motionf.h \
    edvsd_analyzer.h \
    helper/eventf.h \
    genetricalgorithm_driver/geneticalgorithm_driver.h \
    geneticalgorithm/geneticalgorithm.h \
	growingneuralgas/growingneuralgas.h \
    growingneuralgar_driver/growingneuralgas_driver.h \
	helper/random.h \
    dyntracker/dyntracker.h \
    helper/feature_event.h

FORMS    += edvsd_viewer.ui
