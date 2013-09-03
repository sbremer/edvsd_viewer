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

QList<MotionF> EDVSD_Anormaly_Detection::analyzeMotionStartpoints(EDVS_Event *p_buffer, int p_n)
{
	const double Tracker_Factor = 2.0;
	const double Tracker_Pow = 3.2;
	const double Seperator_Dist = 1.0;

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
	QList<QPointF> points;
	for(int x=0;x<=12;x++){
		for(int y=0;y<=12;y++){
			if(getDistance(QPointF(3.0+x*10, 3.0+y*10), point_cloud_revers[y*13+x])>2){
				QPointF tmp;
				tmp = point_cloud_revers[y*13+x];
				points.append(tmp);
			}
		}
	}

	QList<MotionF> startpoints;

	//Find startpoints (
	MotionF tmp;
	tmp.start = points.takeFirst();
	tmp.num = 1;
	startpoints.append(tmp);

	while(!points.isEmpty()){
		MotionF tmp;
		tmp.start = points.takeFirst();
		bool found = false;
		for(QList<MotionF>::iterator i = startpoints.begin(); i!= startpoints.end(); i++){
			if(getDistance(tmp.start, i->start)<Seperator_Dist){
				found = true;
				i->num++;
				break;
			}
		}
		if(!found){
			tmp.num = 1;
			startpoints.append(tmp);
		}
	}

	//Filter single start points
	for(QList<MotionF>::iterator i = startpoints.begin(); i!= startpoints.end();){
		if(i->num<2){
			i = startpoints.erase(i);
		}
		else{
			i++;
		}
	}
	return startpoints;
}

QList<MotionF> EDVSD_Anormaly_Detection::analyzeMotionEndpoints(EDVS_Event *p_buffer, int p_n, QList<MotionF> p_motions)
{
	const double Tracker_Factor = 2.0;
	const double Tracker_Pow = 3.2;

	//Track motion (forward, find endpoints)
	for(QList<MotionF>::iterator i = p_motions.begin(); i!= p_motions.end(); i++){
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
	return p_motions;
}

QList<quint32> EDVSD_Anormaly_Detection::analyzeMotion(EDVS_Event *p_buffer, int p_n, QList<MotionF> p_motions)
{
	QList<quint32> endtimestamps;

	//Motion Tracking
	for(QList<MotionF>::iterator i = p_motions.begin(); i!= p_motions.end(); i++){
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
				if(getDistance(*j,i->start)<10.0){
					atstart = true;
				}
			}

			QPointF tmp;
			QPointF point = *pointmin;
			double fact = 3.0/qPow(getDistance(event, point), 2.0);
			fact = qMin(1.0, fact);
			tmp.setX(point.x()*(1.0-fact)+event.x()*fact);
			tmp.setY(point.y()*(1.0-fact)+event.y()*fact);
			*pointmin = tmp;

			if(getDistance(tmp,i->end)<5.0){
				tracker.erase(pointmin);
				endtimestamps.append(p_buffer[a].t);
			}

			if(!atstart){
				tracker.append(i->start);
			}
		}
	}
	return endtimestamps;
}

void EDVSD_Anormaly_Detection::analyzeEvents(EDVS_Event *p_buffer, int p_n)
{
	m_motions = analyzeMotionStartpoints(p_buffer, p_n);

	m_motions = analyzeMotionEndpoints(p_buffer, p_n, m_motions);

	m_endevents = analyzeMotion(p_buffer, p_n, m_motions);

	//draw start and endpoint
	for(QList<MotionF>::iterator i = m_motions.begin(); i!= m_motions.end(); i++){
		m_painter->drawEllipse(i->start,1.0,1.0);
		m_painter->drawEllipse(i->end,1.0,1.0);
		m_painter->drawLine(i->start, i->end);
	}

	for(QList<quint32>::iterator i = m_endevents.begin(); i!=m_endevents.end(); i++){
		//cout << *i << endl;
		if(i!=m_endevents.begin()){
			cout << "\t" << (*i-*(i-1)) << endl;
		}
	}

//	QPointF *tmp = new QPointF[2];
//	tmp[0] = tmp[1] = m_motions.at(0).start;
	tracker.append(Particle2(m_motions.at(0).start));
}

void EDVSD_Anormaly_Detection::analyzeLiveEvents(EDVS_Event *p_buffer, int p_n)
{
	m_painter->fillRect(0,0,128,128,Qt::transparent);

	const double End_Dist = 6;
	const int Points = 2;

	for(QList<MotionF>::iterator i = m_motions.begin(); i!= m_motions.end(); i++){

		m_painter->drawEllipse(i->start,1.0,1.0);
		m_painter->drawEllipse(i->end,1.0,1.0);
		m_painter->drawLine(i->start, i->end);
		//QList<QPointF*> tracker;

//		QPointF *tmp = new QPointF[2];
//		tmp[0] = tmp[1] = i->start;
//		tracker.append(tmp);

		for(int a=0;a<p_n;a++){
			if(!p_buffer[a].p)continue;
			bool atstart = false;

			//Find closest point to event
			QPointF event = QPointF((double)(p_buffer[a].x), (double)(p_buffer[a].y));
			double distmin = 1000;
			QList<Particle2>::iterator pointmin;
			int particlemin;
			for(QList<Particle2>::iterator j = tracker.begin(); j!=tracker.end();j++){
				for(int b=0;b<Points;b++){
					double dist = getDistance(event, j->point[b]);
					if(dist<distmin){
						distmin = dist;
						pointmin = j;
						particlemin = b;
					}
					if(getDistance(j->point[b],i->start)<5.0){
						atstart = true;
					}
				}
			}

//			QPointF tmp;
//			QPointF point = (*pointmin)[particlemin];
//			double fact = 0.2/qPow(getDistance(event, point), 1);
//			//fact = 0.1;
//			fact = qMin(1.0, fact);
//			tmp.setX(point.x()*(1.0-fact)+event.x()*fact);
//			tmp.setY(point.y()*(1.0-fact)+event.y()*fact);
//			(*pointmin)[particlemin] = tmp;

//			QPointF tmp2;
//			QPointF point2 = (*pointmin)[(particlemin+1)%2];
//			double fact2 = 0.1/qPow(getDistance(event, point2), 1);
//			fact2 = 0.02;
//			fact2 = qMin(1.0, fact2);
//			tmp2.setX(point2.x()*(1.0-fact2)+event.x()*fact2);
//			tmp2.setY(point2.y()*(1.0-fact2)+event.y()*fact2);
//			(*pointmin)[(particlemin+1)%2] = tmp2;

			QPointF *p1 = pointmin->point+particlemin;
			QPointF *p2 = pointmin->point+(particlemin+1)%2;

			QPointF delta = event-*p1;
			double fact = 0.2/getDistance(QPointF(0.0,0.0),delta);
			fact = qMin(0.2,fact);

			*p1 += delta*fact;
			*p2 += delta*fact*0.5 + (*p1 - *p2)*0.01;

			if(getDistance(*p1,i->end)+getDistance(*p2,i->end)<End_Dist){
				tracker.erase(pointmin);
				//endtimestamps.append(p_buffer[a].t);
			}

			if(!atstart){
				tracker.append(Particle2(m_motions.at(0).start));
			}
		}

		for(QList<Particle2>::iterator j = tracker.begin(); j!=tracker.end();j++){
			m_painter->drawEllipse(j->point[0],1.0,1.0);
			m_painter->drawEllipse(j->point[1],1.0,1.0);
			m_painter->drawLine(j->point[0], j->point[1]);
		}
	}

}

double EDVSD_Anormaly_Detection::getDistance(QPointF p_p1, QPointF p_p2)
{
	return qSqrt((p_p1.x()-p_p2.x())*(p_p1.x()-p_p2.x())+(p_p1.y()-p_p2.y())*(p_p1.y()-p_p2.y()));
}
