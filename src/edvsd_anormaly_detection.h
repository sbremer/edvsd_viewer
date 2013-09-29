#ifndef EDVSD_ANORMALY_DETECTION_H
#define EDVSD_ANORMALY_DETECTION_H

#define QT

#include <QPainter>
#include <qmath.h>

#include "edvsd.h"
#include "helper/pointf.h"
#include "kohonentracking/kohonentracking.h"
#include "neuralnet_driver/neuralnet_driver.h"

struct MotionF{
	QPointF start,end;
	int num;

	bool operator==(MotionF a) const{
		if(a.start.x()==start.x() && a.start.y()==start.y())return true;
		else return false;
	}
};

class EDVSD_Anormaly_Detection : public QObject
{
	Q_OBJECT

public:
	explicit EDVSD_Anormaly_Detection(QObject *parent = 0);
	~EDVSD_Anormaly_Detection();

	void setDebugPainter(QPainter *p_painter);

	void dumpNNData();

	void analyzeEvents(EDVS_Event *p_buffer, int p_n);

public slots:
	void analyzeLiveEvents(EDVS_Event *p_buffer, int p_n);

private:
	QPainter *m_painter;

	QList<MotionF> m_motions;

	double getDistance(QPointF,QPointF);

	QList<quint32> m_endevents;

	QList<MotionF> analyzeMotionStartpoints(EDVS_Event *p_buffer, int p_n);
	QList<MotionF> analyzeMotionEndpoints(EDVS_Event *p_buffer, int p_n, QList<MotionF> p_motions);
	QList<quint32> analyzeMotion(EDVS_Event *p_buffer, int p_n, QList<MotionF> p_motions);

	KohonenTracking<2> m_tracking;
	NeuralNet_Driver m_neuralnet_x, m_neuralnet_y, m_neuralnet_atan;
	int m_time_comp;

	GnuPlot_Output m_output_xy, m_output_nn, m_output_error;
};

#endif // EDVSD_ANORMALY_DETECTION_H
