#ifndef EDVSD_ANORMALY_DETECTION_H
#define EDVSD_ANORMALY_DETECTION_H

#include <QPainter>
#include <qmath.h>

#include "edvsd.h"

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

	void setDebugPainter(QPainter *p_painter);

	void analyzeEvents(EDVS_Event *p_buffer, int p_n);

public slots:
	void analyzeLiveEvents(EDVS_Event *p_buffer, int p_n);

private:
	QPainter *m_painter;

	QList<MotionF> m_motions;

	double getDistance(QPointF,QPointF);

	const double Tracker_Factor = 2.0;
	const double Tracker_Pow = 4.0;
	const double Seperator_Dist = 1.0;

};

#endif // EDVSD_ANORMALY_DETECTION_H
