#ifndef EDVSD_ANORMALY_DETECTION_H
#define EDVSD_ANORMALY_DETECTION_H

#define QT

#include <QPainter>
#include <qmath.h>

#include "edvsd.h"
#include "helper/pointf.h"
#include "helper/motionf.h"
#include "startendtracker/startendtracker.h"
#include "kohonentracking/kohonentracking.h"
#include "neuralnet_driver/neuralnet_driver.h"
#include "growingneuralgar_driver/growingneuralgas_driver.h"

class EDVSD_Anormaly_Detection : public QObject
{
	Q_OBJECT

public:
	explicit EDVSD_Anormaly_Detection(QObject *parent = 0);
	explicit EDVSD_Anormaly_Detection(QObject *parent, vector<double> p_tracking_param);
	~EDVSD_Anormaly_Detection();

	void setDebugPainter(QPainter *p_painter);

	void dumpNNData();

	void analyzeEvents(EDVS_Event *p_buffer, int p_n);
	void testEvents(EDVS_Event *p_buffer, int p_n);

public slots:
	void analyzeLiveEvents(EDVS_Event *p_buffer, int p_n);
	void testLiveEvents(EDVS_Event *p_buffer, int p_n);

private:
	QPainter *m_painter;

	StartEndTracker m_startendtracker;

	QList<MotionF> m_motions;

	double getDistance(QPointF,QPointF);

	QList<quint32> m_endevents;

	vector<double> m_tracking_param;
	KohonenTracking<2> m_tracking;
	NeuralNet_Driver m_neuralnet_x, m_neuralnet_y, m_neuralnet_atan;
	int m_time_comp;

	const int m_gngd_dimension;
	GrowingNeuralGas_Driver m_gngd;

	const double m_error_reduction;

	GnuPlot_Output m_output_xy, m_output_nn, m_output_error;
};

#endif // EDVSD_ANORMALY_DETECTION_H
