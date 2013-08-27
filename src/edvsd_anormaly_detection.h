#ifndef EDVSD_ANORMALY_DETECTION_H
#define EDVSD_ANORMALY_DETECTION_H

#include <QPainter>
#include <qmath.h>

#include "edvsd.h"

class EDVSD_Anormaly_Detection : public QObject
{
	Q_OBJECT

public:
	explicit EDVSD_Anormaly_Detection(QObject *parent = 0);

	void setDebugPainter(QPainter *p_painter);

public slots:
	void analyzeEvents(EDVS_Event *p_buffer, int p_n);

private:
	QPainter *m_painter;
	QPointF m_points[13*13];
};

#endif // EDVSD_ANORMALY_DETECTION_H
