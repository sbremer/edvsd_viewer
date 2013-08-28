#include "edvsd_anormaly_detection.h"

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

//	//Normalize Timestamp (first ts = 0; 24bit TS ~ 16.7s, plenty --> no overflows)
//	quint32 ts_norm = p_buffer[0].t;
//	for(int a=0;a<p_n;a++){
//		p_buffer[a].t = 0xFFFFFF & (p_buffer[a].t - ts_norm);
//	}

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

	QList<QPointF> point_cloud;

	for(QList<MotionF>::iterator i = m_motions.begin(); i!= m_motions.end(); i++){
		point_cloud.append(i->start);
	}

	//Track motion (forward, find endpoints)
	for(QList<QPointF>::iterator i = point_cloud.begin(); i != point_cloud.end(); i++){
		for(int a=0;a<p_n;a++){
			EDVS_Event event = p_buffer[a];
			QPointF tmp;
			QPointF point = *i;
			double fact = Tracker_Factor/qPow(qSqrt((event.x-point.x())*(event.x-point.x())+(event.y-point.y())*(event.y-point.y())), Tracker_Pow);
			fact = qMin(1.0, fact);
			tmp.setX(point.x()*(1.0-fact)+event.x*fact);
			tmp.setY(point.y()*(1.0-fact)+event.y*fact);
			//m_painter->drawLine(*i,tmp);
			*i = tmp;
		}
	}

	//Add endpoints to motion list
	QList<QPointF>::iterator j = point_cloud.begin();
	for(QList<MotionF>::iterator i = m_motions.begin(); i!= m_motions.end(); i++){
		i->end = *j;
		j++;

		m_painter->drawEllipse(i->start,1.0,1.0);
		m_painter->drawEllipse(i->end,1.0,1.0);
		m_painter->drawLine(i->start, i->end);

	}

}

void EDVSD_Anormaly_Detection::analyzeLiveEvents(EDVS_Event *p_buffer, int p_n)
{

}

double EDVSD_Anormaly_Detection::getDistance(QPointF p_p1, QPointF p_p2)
{
	return qSqrt((p_p1.x()-p_p2.x())*(p_p1.x()-p_p2.x())+(p_p1.y()-p_p2.y())*(p_p1.y()-p_p2.y()));
}
