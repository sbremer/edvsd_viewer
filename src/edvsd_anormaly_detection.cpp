#include "edvsd_anormaly_detection.h"

#include <iostream>
using namespace std;

EDVSD_Anormaly_Detection::EDVSD_Anormaly_Detection(QObject *parent)
	:QObject(parent), m_neuralnet_x(), m_neuralnet_y(), m_neuralnet_atan(), m_output_xy("data.dat"), m_output_nn("data2.dat"), m_output_error("error.dat"),
	  m_startendtracker(), m_gngd_dimension(5), m_gngd(m_gngd_dimension), m_error_reduction(0.04)
{

}

EDVSD_Anormaly_Detection::EDVSD_Anormaly_Detection(QObject *parent, vector<double> p_tracking_param)
	:QObject(parent), m_neuralnet_x(), m_neuralnet_y(), m_neuralnet_atan(), m_output_xy("data.dat"), m_output_nn("data2.dat"), m_output_error("error.dat"),
	  m_tracking_param(p_tracking_param), m_gngd_dimension(5), m_gngd(m_gngd_dimension), m_error_reduction(0.01),

	  m_startendtracker(p_tracking_param[0], p_tracking_param[1], p_tracking_param[2], p_tracking_param[3], p_tracking_param[4]),
	  m_tracking(p_tracking_param[5], p_tracking_param[6], p_tracking_param[7], p_tracking_param[8], p_tracking_param[9], p_tracking_param[10])
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
		m_output_nn.writeData(4, t, m_neuralnet_x.calculate(t), m_neuralnet_y.calculate(t), m_neuralnet_atan.calculate(t));
		t += 0.05;
	}

	m_output_xy.flush();
	m_output_nn.flush();

//	system("gnuplot -p -e \"load 'plot_xy.plt';\"");
//	system("gnuplot -p -e \"load 'plot_tan.plt';\"");
//	system("gnuplot -p -e \"load 'plot_error.plt';\"");
}

void EDVSD_Anormaly_Detection::analyzeEvents(EventF *p_buffer, int p_n)
{
	return;

	list<MotionF> motions = m_startendtracker.trackPoints(p_buffer, p_n);

	for(list<MotionF>::iterator i = motions.begin(); i != motions.end(); i++){
		m_motions.append(*i);
	}

//	cout << m_motions.size() << endl;

//	//draw start and endpoint
//	for(QList<MotionF>::iterator i = m_motions.begin(); i!= m_motions.end(); i++){
//		m_painter->drawEllipse(i->start.toQPointF(),1.0,1.0);
//		m_painter->drawEllipse(i->end.toQPointF(),1.0,1.0);
//		m_painter->drawLine(i->start.toQPointF(), i->end.toQPointF());
//	}

	//return;

//	for(QList<quint32>::iterator i = m_endevents.begin(); i!=m_endevents.end(); i++){
//		//cout << *i << endl;
//		if(i!=m_endevents.begin()){
//			//cout << "\t" << (*i-*(i-1)) << endl;
//		}
//	}

	m_time_comp = -1;
	m_tracking.initialize(PointF(m_motions.at(0).start), PointF(m_motions.at(0).end), true);

	int writeerror = -1;

	for(int a = 0; a < p_n; a++){
		const KohonenMap<2>* map = m_tracking.analyzeEvent(p_buffer[a]);

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
			double t = (double)p_buffer[a].ts - map->ts;

			x = (x - 64) / 64;
			y = (y - 64) / 64;
			atan = atan;
			t = (t - m_time_comp / 2.0) / (m_time_comp / 4.0);


			//double distmin = m_tracking.getTrackerDistance(map);

			//m_output_xy.writeData(5, t, x, y, atan, distmin);

			vector<double> data(m_gngd_dimension);
			int i = 0;

            data[i++] = x;
            data[i++] = y;
            data[i++] = atan / 2.0;
            data[i++] = m_tracking.getTrackerDistance(map) / 128.0;
            data[i++] = PointF::getDistance(map->points[0], map->points[1]) / 128.0;


//			data[0] = x;
//			data[1] = y;
//            //data[2] = atan;
//            //data[2] = (double)m_tracking.getListLength() / 10.0;
//            data[2] = m_tracking.getTrackerDistance(map) / 128.0;
//            data[3] = PointF::getDistance(map->points[0], map->points[1]) / 128.0;
//            data[4] = atan;

			m_gngd.learn(data);

			m_output_xy.writeData(data);

//			m_neuralnet_x.train(t, x);
//			m_neuralnet_y.train(t, y);
//			m_neuralnet_atan.train(t, atan);

//			if(a > writeerror + 50){
//				m_output_error.writeData(7, (double)(a-writeerror), m_neuralnet_x.getLastError(), m_neuralnet_y.getLastError(), m_neuralnet_x.getPerformance(), m_neuralnet_y.getPerformance(), m_neuralnet_atan.getLastError(), m_neuralnet_atan.getPerformance());
//			}
		}
	}

	//dumpNNData();

	m_gngd.dumpData();
	m_output_xy.flush();

	//system("gnuplot -p -e \"load 'plot_xy.plt';\"");
	system("gnuplot -p -e \"load 'plot_gng3.plt';\"");

	m_time_comp = -1;
	m_tracking.initialize(PointF(m_motions.at(0).start), PointF(m_motions.at(0).end), true);
}

void EDVSD_Anormaly_Detection::analyzeLiveEvents(EventF *p_buffer, int p_n)
{
	for(int a = 0; a < p_n; a++){
		if(p_buffer[a].polarity == false){
			continue;
		}

		m_dyntracker.analyzeEvent(p_buffer[a]);
	}

	m_painter->fillRect(0,0,128,128,Qt::transparent);

	for(list<TrackingUnit>::const_iterator iter = m_dyntracker.getTrackers().begin(); iter != m_dyntracker.getTrackers().end(); iter++){
		for(list<TrackingNode>::const_iterator jter = iter->nodes.begin(); jter != iter->nodes.end(); jter++){
			m_painter->drawEllipse(PointF::toQPointF(jter->point), 1.0, 1.0);
		}
	}

	m_painter->drawEllipse(PointF::toQPointF(m_dyntracker.getInitialTracker()), 0.5, 0.5);

	return;

	m_painter->fillRect(0,0,128,128,Qt::transparent);

	//draw start and endpoint
	for(QList<MotionF>::iterator i = m_motions.begin(); i!= m_motions.end(); i++){
		m_painter->drawEllipse(i->start.toQPointF(),1.0,1.0);
		m_painter->drawEllipse(i->end.toQPointF(),1.0,1.0);
		m_painter->drawLine(i->start.toQPointF(), i->end.toQPointF());
	}

	for(int a = 0; a < p_n; a++){
		const KohonenMap<2>* map = m_tracking.analyzeEvent(p_buffer[a]);
		//const KohonenMap<2>* map = m_tracking.analyzeEvent(PointF((double)(p_buffer[a].x), (double)(p_buffer[a].y)), (bool)(p_buffer[a].p), (unsigned int)(p_buffer[a].t));
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

void EDVSD_Anormaly_Detection::testEvents(EventF *p_buffer, int p_n)
{
	list<MotionF> motions = m_startendtracker.trackPoints(p_buffer, p_n);

	m_motions.clear();

	for(list<MotionF>::iterator i = motions.begin(); i != motions.end(); i++){
		m_motions.append(*i);
	}

	m_time_comp = -1;
	m_tracking.initialize(PointF(m_motions.at(0).start), PointF(m_motions.at(0).end), true);

	m_output_error.clear();

	int errorsamples = 0;
	int samples = 0;

	for(int a = 0; a < p_n; a++){
		KohonenMap<2>* map = m_tracking.analyzeEvent(p_buffer[a]);

		if(map == 0){
			continue;
		}
		samples++;

		if(m_tracking.getDurationMin() < 1000000 && m_time_comp == -1){
			m_time_comp = m_tracking.getDurationMin();
		}

		if(m_time_comp != -1 && map->ts != -1 && p_buffer[a].ts - map->ts > 2000){
			double x = (map->points[0].x + map->points[1].x) / 2.0;
			double y = (map->points[0].y + map->points[1].y) / 2.0;
			double atan = qAtan((map->points[0].x - map->points[1].x) / (map->points[0].y - map->points[1].y));
			double t = (double)p_buffer[a].ts - map->ts;

			x = (x - 64) / 64;
			y = (y - 64) / 64;
			atan = atan;
			t = (t - m_time_comp / 2.0) / (m_time_comp / 4.0);


			//double distmin = m_tracking.getTrackerDistance(map);

			//m_output_xy.writeData(5, t, x, y, atan, distmin);

            vector<double> data(m_gngd_dimension);
			int i = 0;

			data[i++] = x;
			data[i++] = y;
            data[i++] = atan / 2.0;
			data[i++] = m_tracking.getTrackerDistance(map) / 128.0;
			data[i++] = PointF::getDistance(map->points[0], map->points[1]) / 128.0;

			double error = m_gngd.test(data);
			//error = error * error;
			map->error = m_error_reduction * error + (1.0 - m_error_reduction) * map->error;
			//map->error = error;

			map->errorsum += map->error;
			map->events++;

			//m_output_xy.writeData(data);
			//if(map->error > 0.015)
			m_output_error.writeData(1, map->error);
			if(map->error > 0.03)errorsamples++;
		}
	}

	m_output_error.flush();
	system("gnuplot -p -e \"load 'plot_error_hist.plt';\"");

	cout << "Error: " << (double)errorsamples / (double)samples << endl;

	m_time_comp = -1;
	m_tracking.initialize(PointF(m_motions.at(0).start), PointF(m_motions.at(0).end), true);

	m_painter->setFont(QFont("Helvetica", 3));
}

void EDVSD_Anormaly_Detection::testLiveEvents(EventF *p_buffer, int p_n)
{
	m_painter->fillRect(0,0,128,128,Qt::transparent);

	for(int a = 0; a < p_n; a++){
		KohonenMap<2>* map = m_tracking.analyzeEvent(p_buffer[a]);

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
			double t = (double)p_buffer[a].ts - map->ts;

			x = (x - 64) / 64;
			y = (y - 64) / 64;
			atan = atan;
			t = (t - m_time_comp / 2.0) / (m_time_comp / 4.0);


			//double distmin = m_tracking.getTrackerDistance(map);

			//m_output_xy.writeData(5, t, x, y, atan, distmin);
			vector<double> data(m_gngd_dimension);
			int i = 0;

            data[i++] = x;
            data[i++] = y;
            data[i++] = atan / 2.0;
            data[i++] = m_tracking.getTrackerDistance(map) / 128.0;
            data[i++] = PointF::getDistance(map->points[0], map->points[1]) / 128.0;

			double error = m_gngd.test(data);
			map->error = m_error_reduction * error + (1.0 - m_error_reduction) * map->error;
			//map->error = error;

			//m_output_xy.writeData(data);
		}
	}
	if(m_time_comp != -1){
		for(int a = 0; a < m_tracking.getListLength(); a++){
			KohonenMap<2>* iter = 0;
			iter = m_tracking.getKohonenMap(a);
			if(m_tracking.getKohonenMap(a)->ts == -1 || m_tracking.getKohonenMap(a)->error < 0.03){
				continue;
			}

			m_painter->drawEllipse(PointF::toQPointF(m_tracking.getKohonenMap(a)->points[0]), 1.0, 1.0);
			m_painter->drawEllipse(PointF::toQPointF(m_tracking.getKohonenMap(a)->points[1]), 1.0, 1.0);
			m_painter->drawLine(PointF::toQPointF(m_tracking.getKohonenMap(a)->points[0]), PointF::toQPointF(m_tracking.getKohonenMap(a)->points[1]));
			m_painter->drawText(PointF::toQPointF(m_tracking.getKohonenMap(a)->points[1]), QString::number(m_tracking.getKohonenMap(a)->error, 'f', 3));
		}
	}
}

double EDVSD_Anormaly_Detection::getDistance(QPointF p_p1, QPointF p_p2)
{
	return qSqrt((p_p1.x()-p_p2.x())*(p_p1.x()-p_p2.x())+(p_p1.y()-p_p2.y())*(p_p1.y()-p_p2.y()));
}
