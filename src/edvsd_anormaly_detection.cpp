#include "edvsd_anormaly_detection.h"

#include <iostream>
using namespace std;

EDVSD_Anormaly_Detection::EDVSD_Anormaly_Detection(QObject *parent) :
	QObject(parent)
{
}

void EDVSD_Anormaly_Detection::setDebugPainter(QPainter *p_painter)
{
	m_painter = p_painter;
}

void EDVSD_Anormaly_Detection::analyzeEvents(EDVS_Event *p_buffer, int p_n)
{
	m_painter->fillRect(0,0,128,128,Qt::transparent);

	QPointF point_cloud_revers[13*13];

	for(int x=0;x<=12;x++){
		for(int y=0;y<=12;y++){
			point_cloud_revers[y*13+x] = QPointF(3.0+x*10, 3.0+y*10);
		}
	}

	//Track Motion (revers to find motion startpoint)
	for(int a=p_n-1;a>=0;a--){
		if(!p_buffer[a].p)continue;
		for(int x=0;x<=12;x++){
			for(int y=0;y<=12;y++){
				EDVS_Event event = p_buffer[a];
				QPointF tmp;
				QPointF point = point_cloud_revers[y*13+x];
				double fact = Tracker_Factor/qPow(qSqrt((event.x-point.x())*(event.x-point.x())+(event.y-point.y())*(event.y-point.y())), Tracker_Pow);
				fact = qMin(1.0, fact);
				tmp.setX(point.x()*(1.0-fact)+event.x*fact);
				tmp.setY(point.y()*(1.0-fact)+event.y*fact);
				//m_painter->drawLine(QPointF(3.0+x*10, 3.0+y*10),tmp);
				point_cloud_revers[y*13+x] = tmp;
			}
		}
	}

	//Filter moved points
	QList<MotionF> points;
	for(int x=0;x<=12;x++){
		for(int y=0;y<=12;y++){
			if(getDistance(QPointF(3.0+x*10, 3.0+y*10), point_cloud_revers[y*13+x])>2){
				MotionF tmp;
				tmp.start = point_cloud_revers[y*13+x];
				//tmp.end = QPointF(3.0+x*10, 3.0+y*10);
				points.append(tmp);
			}
		}
	}

	//Find endpoints
	MotionF tmp = points.takeFirst();
	tmp.num = 1;
	m_motions.append(tmp);

	while(!points.isEmpty()){
		MotionF tmp = points.takeFirst();
		bool found = false;
		for(QList<MotionF>::iterator i = m_motions.begin(); i!= m_motions.end(); i++){
			if(getDistance(tmp.start, i->start)<Seperator_Dist){
				found = true;
				i->num++;
				break;
			}
		}
		if(!found){
			tmp.num = 1;
			m_motions.append(tmp);
		}
	}

	//Filter single start points
	for(QList<MotionF>::iterator i = m_motions.begin(); i!= m_motions.end();){
		if(i->num<2){
			i = m_motions.erase(i);
		}
		else{
			i++;
		}
	}

	//Track motion (forward, find endpoints)
	for(QList<MotionF>::iterator i = m_motions.begin(); i!= m_motions.end(); i++){
		QPointF tracker = i->start;

		for(int a=0;a<p_n;a++){
			if(!p_buffer[a].p)continue;
			EDVS_Event event = p_buffer[a];
			QPointF tmp;
			QPointF point = tracker;
			double fact = Tracker_Factor/qPow(qSqrt((event.x-point.x())*(event.x-point.x())+(event.y-point.y())*(event.y-point.y())), Tracker_Pow);
			fact = qMin(1.0, fact);
			tmp.setX(point.x()*(1.0-fact)+event.x*fact);
			tmp.setY(point.y()*(1.0-fact)+event.y*fact);
			//m_painter->drawLine(*i,tmp);
			tracker = tmp;
		}

		i->end = tracker;
	}

	//draw start and endpoint
	for(QList<MotionF>::iterator i = m_motions.begin(); i!= m_motions.end(); i++){
		m_painter->drawEllipse(i->start,1.0,1.0);
		m_painter->drawEllipse(i->end,1.0,1.0);
		m_painter->drawLine(i->start, i->end);
	}

	//Motion Tracking
	for(QList<MotionF>::iterator i = m_motions.begin(); i!= m_motions.end(); i++){
		QList<QPointF> tracker;
		tracker.append(i->start);

		for(int a=0;a<p_n;a++){
			if(!p_buffer[a].p)continue;
			bool atstart = false;

			//Find closest point to event
			QPointF event = QPointF((double)(p_buffer[a].x), (double)(p_buffer[a].y));
			double distmin = 1000;
			QList<QPointF>::iterator pointmin;
			for(QList<QPointF>::iterator j = tracker.begin(); j!=tracker.end();j++){
				double dist = getDistance(event, *j);
				if(dist<distmin){
					distmin = dist;
					pointmin = j;
				}
				if(getDistance(*j,i->start)<8.0){
					atstart = true;
				}
			}

			QPointF tmp;
			QPointF point = *pointmin;
			double fact = 2*Tracker_Factor/qPow(getDistance(event, point), 2.5);
			fact = qMin(1.0, fact);
			tmp.setX(point.x()*(1.0-fact)+event.x()*fact);
			tmp.setY(point.y()*(1.0-fact)+event.y()*fact);
			*pointmin = tmp;

			if(getDistance(tmp,i->end)<5.0){
				tracker.erase(pointmin);
				m_endevents.append(p_buffer[a].t);
			}

			if(!atstart){
				tracker.append(i->start);
			}
		}
	}

	for(QList<quint32>::iterator i = m_endevents.begin(); i!=m_endevents.end(); i++){
		//cout << *i << endl;
		if(i!=m_endevents.begin()){
			cout << "\t" << (*i-*(i-1)) << endl;
		}
	}

}

void EDVSD_Anormaly_Detection::analyzeLiveEvents(EDVS_Event *p_buffer, int p_n)
{

}

double EDVSD_Anormaly_Detection::getDistance(QPointF p_p1, QPointF p_p2)
{
	return qSqrt((p_p1.x()-p_p2.x())*(p_p1.x()-p_p2.x())+(p_p1.y()-p_p2.y())*(p_p1.y()-p_p2.y()));
}
