#include "edvsd_anormaly_detection.h"

EDVSD_Anormaly_Detection::EDVSD_Anormaly_Detection(QObject *parent) :
	QObject(parent)
{
	for(int x=0;x<=12;x++){
		for(int y=0;y<=12;y++){
			m_points[y*13+x] = QPointF(3.0+x*10, 3.0+y*10);
		}
	}
}

void EDVSD_Anormaly_Detection::setDebugPainter(QPainter *p_painter)
{
	m_painter = p_painter;
}

void EDVSD_Anormaly_Detection::analyzeEvents(EDVS_Event *p_buffer, int p_n)
{
	m_painter->fillRect(0,0,128,128,Qt::transparent);
	for(int a=0;a<p_n;a++){
		if(!p_buffer[a].p)continue;
		for(int x=0;x<=12;x++){
			for(int y=0;y<=12;y++){
				EDVS_Event event = p_buffer[a];
				QPointF tmp;
				QPointF point = m_points[y*13+x];
				double fact = 1.0/qPow(qSqrt((event.x-point.x())*(event.x-point.x())+(event.y-point.y())*(event.y-point.y())),4);
				fact = qMin(1.0, fact);
				tmp.setX(point.x()*(1.0-fact)+event.x*fact);
				tmp.setY(point.y()*(1.0-fact)+event.y*fact);
				//m_painter->drawLine(QPointF(3.0+x*10, 3.0+y*10),tmp);
				m_points[y*13+x] = tmp;
			}
		}
	}

}
