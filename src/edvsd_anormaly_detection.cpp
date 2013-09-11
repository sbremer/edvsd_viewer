#include "edvsd_anormaly_detection.h"

#include <iostream>
using namespace std;

EDVSD_Anormaly_Detection::EDVSD_Anormaly_Detection(QObject *parent)
	:QObject(parent), m_neuralnet_x(2, NeuralNet_XY, 1), m_neuralnet_y(2, NeuralNet_XY, 1), m_neuralnet_atan(2, NeuralNet_ATan, 1)//, m_tracking()
{
	system("rm data.dat");
	m_output_file.setFileName("data.dat");
	m_output_file.open(QIODevice::WriteOnly);

	system("rm data2.dat");
	m_output_file2.setFileName("data2.dat");
	m_output_file2.open(QIODevice::WriteOnly);
}

EDVSD_Anormaly_Detection::~EDVSD_Anormaly_Detection()
{
	m_output_file.close();
	m_output_file2.close();
}

int EDVSD_Anormaly_Detection::NeuralNet_XY[2] = {2, 1};
int EDVSD_Anormaly_Detection::NeuralNet_ATan[2] = {4, 1};

void EDVSD_Anormaly_Detection::setDebugPainter(QPainter *p_painter)
{
	m_painter = p_painter;
}

void EDVSD_Anormaly_Detection::dumpNNData()
{
	double t = -2.2;

	while(t <= 2.2){
		double x = m_neuralnet_x.calculate(&t);
		double y = m_neuralnet_y.calculate(&t);
		double atan = 0;//m_neuralnet_atan.calculate(&t);

		QString cmd;
		cmd += QString::number(t) + "\t";
		cmd += QString::number(x) + "\t";
		cmd += QString::number(y) + "\t";
		cmd += QString::number(atan) + "\n";
		m_output_file2.write(cmd.toLocal8Bit().data());

		t += 0.1;
	}
	m_output_file2.flush();
	cout << "Dumped" << endl;
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

//	QPointF *tmp = new QPointF[2];
//	tmp[0] = tmp[1] = m_motions.at(0).start;
//	tracker.append(Particle2(m_motions.at(0).start));
//	spawn = -1;

	m_time_comp = -1;
	m_tracking.initialize(PointF(m_motions.at(0).start), PointF(m_motions.at(0).end), true);
	m_neuralnet_x.initialize(1.0);
	m_neuralnet_y.initialize(1.0);
	//m_neuralnet_atan.initialize(1.0);

	for(int a = 0; a < p_n; a++){
		const KohonenMap<2>* map = m_tracking.analyzeEvent(PointF((double)(p_buffer[a].x), (double)(p_buffer[a].y)), (bool)(p_buffer[a].p), (unsigned int)(p_buffer[a].t));

		if(map == 0){
			continue;
		}

		if(m_tracking.getDurationMin() < 1000000 && m_time_comp == -1){
			m_time_comp = m_tracking.getDurationMin();
		}

		if(m_time_comp != -1 && map->ts != -1){
			double x = (map->points[0].x + map->points[1].x) / 2.0;
			double y = (map->points[0].y + map->points[1].y) / 2.0;
			double atan = qAtan((map->points[0].x - map->points[1].x) / (map->points[0].y - map->points[1].y));
			double t = (double)p_buffer[a].t - map->ts;

			x = (x - 64) / 32;
			y = (y - 64) / 32;
			atan = atan;
			t = (t - m_time_comp / 2.0) / (m_time_comp / 4);

			QString cmd;
			cmd += QString::number(t) + "\t";
			cmd += QString::number(x) + "\t";
			cmd += QString::number(y) + "\t";
			cmd += QString::number(atan) + "\n";
			m_output_file.write(cmd.toLocal8Bit().data());

			m_neuralnet_x.train(&t, &x);
			m_neuralnet_y.train(&t, &y);
			//m_neuralnet_atan.train(&t, &atan);


		}
	}
	dumpNNData();
}

void EDVSD_Anormaly_Detection::analyzeLiveEvents(EDVS_Event *p_buffer, int p_n)
{
	return;
	m_painter->fillRect(0,0,128,128,Qt::transparent);

	for(int a = 0; a < p_n; a++){
		const KohonenMap<2>* map = m_tracking.analyzeEvent(PointF((double)(p_buffer[a].x), (double)(p_buffer[a].y)), (bool)(p_buffer[a].p), (unsigned int)(p_buffer[a].t));

		if(map == 0){
			continue;
		}

		if(m_tracking.getDurationMin() < 1000000 && m_time_comp == -1){
			m_time_comp = m_tracking.getDurationMin();
		}

		if(m_time_comp != -1 && map->ts != -1){
			double x = (map->points[0].x + map->points[1].x) / 2.0;
			double y = (map->points[0].y + map->points[1].y) / 2.0;
			double atan = qAtan((map->points[0].x - map->points[1].x) / (map->points[0].y - map->points[1].y));
			double t = (double)p_buffer[a].t - map->ts;

			x = (x - 64) / 32;
			y = (y - 64) / 32;
			atan = atan;
			t = (t - m_time_comp / 2.0) / (m_time_comp / 4);

			QString cmd;
			cmd += QString::number(t) + "\t";
			cmd += QString::number(x) + "\t";
			cmd += QString::number(y) + "\t";
			cmd += QString::number(atan) + "\n";
			m_output_file.write(cmd.toLocal8Bit().data());

//			m_neuralnet_x.train(&t, &x);
//			m_neuralnet_y.train(&t, &y);
//			m_neuralnet_atan.train(&t, &atan);


		}
	}

	for(int a = 0; a < m_tracking.getListLength(); a++){
		m_painter->drawEllipse(PointF::toQPointF(m_tracking.getKohonenMap(a)->points[0]), 1.0, 1.0);
		m_painter->drawEllipse(PointF::toQPointF(m_tracking.getKohonenMap(a)->points[1]), 1.0, 1.0);
		m_painter->drawLine(PointF::toQPointF(m_tracking.getKohonenMap(a)->points[0]), PointF::toQPointF(m_tracking.getKohonenMap(a)->points[1]));
	}
}

/*
void EDVSD_Anormaly_Detection::analyzeLiveEvents(EDVS_Event *p_buffer, int p_n)
{
	m_painter->fillRect(0,0,128,128,Qt::transparent);

	const double End_Dist = 6.0;
	const double Start_Dist = 6.0;
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
					if(getDistance(j->point[b],i->start)<2.0){
						atstart = true;
					}
				}
			}

			QPointF *p1 = pointmin->point+particlemin;
			QPointF *p2 = pointmin->point+(particlemin+1)%2;

			QPointF delta = event-*p1;
			double fact = 3.0/qPow(getDistance(QPointF(0.0,0.0),delta),3);
			fact = qMin(0.3,fact);

			*p1 += delta*fact;
			//*p2 += delta*fact*0.5 + (*p1 - *p2)*0.01;
			*p2 += delta*fact*0.6 + (*p1 - *p2)*0.005;

			if(getDistance(*p1,i->start)+getDistance(*p2,i->start)>Start_Dist && pointmin->ts==-1){
				pointmin->ts = p_buffer[a].t;
			}

			if(pointmin->ts!=-1){
				QString cmd;
				cmd += QString::number(p_buffer[a].t-pointmin->ts) + "\t";
				cmd += QString::number((p1->x()+p2->x())/2.0) + "\t";
				cmd += QString::number((p1->y()+p2->y())/2.0) + "\t";
				cmd += QString::number(qAtan((p1->x()-p2->x())/(p1->y()-p2->y()))) + "\n";
				m_output_file.write(cmd.toLocal8Bit().data());
			}

			if(getDistance(*p1,i->end)+getDistance(*p2,i->end)<End_Dist){
				tracker.erase(pointmin);
				//endtimestamps.append(p_buffer[a].t);
			}

			if(!atstart && spawn == -1){
				spawn = p_buffer[a].t + 2000;
				//tracker.append(Particle2(m_motions.at(0).start));
			}
			if(spawn != -1 && p_buffer[a].t >= spawn){
				spawn = -1;
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
*/

double EDVSD_Anormaly_Detection::getDistance(QPointF p_p1, QPointF p_p2)
{
	return qSqrt((p_p1.x()-p_p2.x())*(p_p1.x()-p_p2.x())+(p_p1.y()-p_p2.y())*(p_p1.y()-p_p2.y()));
}
