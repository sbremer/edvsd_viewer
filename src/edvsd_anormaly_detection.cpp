#include "edvsd_anormaly_detection.h"

#include <iostream>
using namespace std;

EDVSD_Anormaly_Detection::EDVSD_Anormaly_Detection(QObject *parent)
	:QObject(parent), m_neuralnet_x(), m_neuralnet_y(), m_neuralnet_atan(), m_output_xy("data.dat"), m_output_nn("data2.dat"), m_output_error("error.dat")
{

}

EDVSD_Anormaly_Detection::~EDVSD_Anormaly_Detection()
{

}

void EDVSD_Anormaly_Detection::setDebugPainter(QPainter *p_painter)
{
	m_painter = p_painter;
}

void EDVSD_Anormaly_Detection::dumpNNData()
{
	double t = -2.2;

	while(t <= 2.6){
		m_output_nn.writeData(3, t, m_neuralnet_x.calculate(t), m_neuralnet_y.calculate(t));
		t += 0.05;
	}

	m_output_xy.flush();
	m_output_nn.flush();
	system("gnuplot -p -e \"load 'plot_xy.plt';\"");
	system("gnuplot -p -e \"load 'plot_error.plt';\"");
}

QList<MotionF> EDVSD_Anormaly_Detection::analyzeMotionStartpoints(EDVS_Event *p_buffer, int p_n)
{
	const double Tracker_Factor = 2.5;
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
	const double Tracker_Pow = 3.3;

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
			//cout << "\t" << (*i-*(i-1)) << endl;
		}
	}

	m_time_comp = -1;
	m_tracking.initialize(PointF(m_motions.at(0).start), PointF(m_motions.at(0).end), true);

	int writeerror = -1;

	for(int a = 0; a < p_n; a++){
		const KohonenMap<2>* map = m_tracking.analyzeEvent(PointF((double)(p_buffer[a].x), (double)(p_buffer[a].y)), (bool)(p_buffer[a].p), (unsigned int)(p_buffer[a].t));

		if(map == 0){
			continue;
		}

		if(m_tracking.getDurationMin() < 1000000 && m_time_comp == -1){
			m_time_comp = m_tracking.getDurationMin();
			writeerror = a;
		}

		if(m_time_comp != -1 && map->ts != -1){
			double x = (map->points[0].x + map->points[1].x) / 2.0;
			double y = (map->points[0].y + map->points[1].y) / 2.0;
			double atan = qAtan((map->points[0].x - map->points[1].x) / (map->points[0].y - map->points[1].y));
			double t = (double)p_buffer[a].t - map->ts;

			x = (x - 64) / 64;
			y = (y - 64) / 64;
			atan = atan;
			t = (t - m_time_comp / 2.0) / (m_time_comp / 4.0);

			m_output_xy.writeData(3, t, x, y);

			m_neuralnet_x.train(t, x);
			m_neuralnet_y.train(t, y);
			//m_neuralnet_atan.train(&t, &atan);

			if(a > writeerror + 50){
				m_output_error.writeData(5, (double)(a-writeerror), m_neuralnet_x.getLastError(), m_neuralnet_y.getLastError(), m_neuralnet_x.getPerformance(), m_neuralnet_y.getPerformance());
			}
		}
	}

	dumpNNData();

	m_time_comp = -1;
	m_tracking.initialize(PointF(m_motions.at(0).start), PointF(m_motions.at(0).end), true);
}

void EDVSD_Anormaly_Detection::analyzeLiveEvents(EDVS_Event *p_buffer, int p_n)
{
	m_painter->fillRect(0,0,128,128,Qt::transparent);

	for(int a = 0; a < p_n; a++){
		const KohonenMap<2>* map = m_tracking.analyzeEvent(PointF((double)(p_buffer[a].x), (double)(p_buffer[a].y)), (bool)(p_buffer[a].p), (unsigned int)(p_buffer[a].t));
//		if(map == 0){
//			continue;
//		}

//		if(m_tracking.getDurationMin() < 1000000 && m_time_comp == -1){
//			m_time_comp = m_tracking.getDurationMin();
//		}

//		if(m_time_comp != -1 && map->ts != -1){
//			double x = (map->points[0].x + map->points[1].x) / 2.0;
//			double y = (map->points[0].y + map->points[1].y) / 2.0;
//			double atan = qAtan((map->points[0].x - map->points[1].x) / (map->points[0].y - map->points[1].y));
//			double t = (double)p_buffer[a].t - map->ts;

//			x = (x - 64) / 32;
//			y = (y - 64) / 32;
//			atan = atan;
//			t = (t - m_time_comp / 2.0) / (m_time_comp / 4);
//		}
	}

	for(int a = 0; a < m_tracking.getListLength(); a++){
		m_painter->drawEllipse(PointF::toQPointF(m_tracking.getKohonenMap(a)->points[0]), 1.0, 1.0);
		m_painter->drawEllipse(PointF::toQPointF(m_tracking.getKohonenMap(a)->points[1]), 1.0, 1.0);
		m_painter->drawLine(PointF::toQPointF(m_tracking.getKohonenMap(a)->points[0]), PointF::toQPointF(m_tracking.getKohonenMap(a)->points[1]));
	}
}

double EDVSD_Anormaly_Detection::getDistance(QPointF p_p1, QPointF p_p2)
{
	return qSqrt((p_p1.x()-p_p2.x())*(p_p1.x()-p_p2.x())+(p_p1.y()-p_p2.y())*(p_p1.y()-p_p2.y()));
}
