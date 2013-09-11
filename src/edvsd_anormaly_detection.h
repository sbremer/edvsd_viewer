#ifndef EDVSD_ANORMALY_DETECTION_H
#define EDVSD_ANORMALY_DETECTION_H

#define QT

#include <QPainter>
#include <qmath.h>

#include <QFile>

#include "edvsd.h"
#include "detection/pointf.h"
#include "detection/kohonentracking.h"
#include "neuralnet/neuralnet.h"

struct MotionF{
	QPointF start,end;
	int num;

	bool operator==(MotionF a) const{
		if(a.start.x()==start.x() && a.start.y()==start.y())return true;
		else return false;
	}
};

/*
template <int N>
struct Particle{
	QPointF point[N];
	int ts;

	Particle(QPointF p_point = QPointF(), int p_ts = -1)
	{
		for(int a=0;a<N;a++){
			point[a] = p_point;
		}
		ts = p_ts;
	}
};
*/

//typedef KohonenMap<2> Particle2;

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
	NeuralNet m_neuralnet_x, m_neuralnet_y, m_neuralnet_atan;
	int m_time_comp;

	QFile m_output_file, m_output_file2;

	static int NeuralNet_XY[2];
	static int NeuralNet_ATan[2];
};

#endif // EDVSD_ANORMALY_DETECTION_H
