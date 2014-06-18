#include "edvsd_anormaly_detection.h"

#include <iostream>
using namespace std;

EDVSD_Anormaly_Detection::EDVSD_Anormaly_Detection(QObject *parent)
	:QObject(parent), m_dyntracker(), m_gngd(m_dyntracker.getFeatureNum())
{
	m_error = 1.0;
}

EDVSD_Anormaly_Detection::~EDVSD_Anormaly_Detection()
{

}

void EDVSD_Anormaly_Detection::setDebugPainter(QPainter *p_painter)
{
	m_painter = p_painter;
}

void EDVSD_Anormaly_Detection::analyzeEvents(EventF *p_buffer, int p_n)
{
	m_collectors = new ErrorCollector*[40];

	double error_imp = 0.005;

	GnuPlot_Output out = GnuPlot_Output(QString("err.dat"));

	for(int a = 0; a < 40; a++){
		m_collectors[a] = NULL;
	}

	//Process all events of this frame, pass them to the tracker
	for(int a = 0; a < p_n; a++){
		//Skip "off" events //Todo
		if(p_buffer[a].polarity == EDVS_Polarity_Off){
			continue;
		}

		m_dyntracker.analyzeEvent(p_buffer[a]);

		FeatureEvent event = m_dyntracker.popFeatureEvent();
		while(event.type != FEATURE_EVENT_TYPE_INVALID){
			double error = m_gngd.processFeatureEvent(event);

			if(error != 0.0){
				m_error = (1.0 - error_imp) * m_error + error_imp * error;

				out.writeData(3, (double)(p_buffer[a].ts) / 1000000, error, m_error);
			}

			switch(event.type){
			case FEATURE_EVENT_TYPE_NEW_NODE:
				//m_collectors[event.id] = new ErrorCollector();
				//m_collectors[event.id]->error += error;
				//m_collectors[event.id]->n++;
				//cout << "New: " << error << endl;
				break;
			case FEATURE_EVENT_TYPE_LEARN_NODE:
				//m_collectors[event.id]->error += error;
				//m_collectors[event.id]->n++;
				break;
			case FEATURE_EVENT_TYPE_KILL_NODE:
				//m_collectors[event.id]->error += error;
				//m_collectors[event.id]->n++;

				//cout << "Dead: " << error << endl;

				//cout << m_collectors[event.id]->error / m_collectors[event.id]->n << endl;

				//delete m_collectors[event.id];
				//m_collectors[event.id] = NULL;
				break;
			}

			event = m_dyntracker.popFeatureEvent();
		}
	}

	m_gngd.dumpData();

	m_dyntracker.resetTracker();

	out.flush();

	//system("gnuplot -p -e \"load 'plot_gng3.plt';\"");
	system("gnuplot -p -e \"load 'plot_error_new.plt';\"");
}

void EDVSD_Anormaly_Detection::analyzeLiveEvents(EventF *p_buffer, int p_n)
{
	double error = 0.0;

	//Process all events of this frame, pass them to the tracker
	for(int a = 0; a < p_n; a++){
		//Skip "off" events //Todo
		if(p_buffer[a].polarity == EDVS_Polarity_Off){
			continue;
		}

		m_dyntracker.analyzeEvent(p_buffer[a]);

		FeatureEvent event = m_dyntracker.popFeatureEvent();
		while(event.type != FEATURE_EVENT_TYPE_INVALID){
			//error = max(error, m_gngd.processFeatureEvent(event));
			event = m_dyntracker.popFeatureEvent();
		}
	}

	//Visual output
	m_painter->fillRect(0,0,128,128,Qt::transparent);

	m_painter->drawText(0,10, QString::number(error));

	for(int a = 0; a < m_dyntracker.getTrackerNum(); a++){
		if(m_dyntracker.isTrackingNodeActive(a)){
			TrackingNode tpoint = m_dyntracker.getTrackingNode(a);

			//Draw nodes
			double radius = 1.8 * sqrt(tpoint.error);
			m_painter->save();
			m_painter->translate(tpoint.position.toQPointF());
			m_painter->rotate(tpoint.angle / M_PI * 180.0);
			m_painter->drawEllipse(QPoint(), radius, 1.0);
			m_painter->restore();

			//Draw node connections
			for(int b = 0; b < a; b++){
				double connection = m_dyntracker.getTrackingNodeConnection(a, b);
				if(connection > 0.8){
					m_painter->drawLine(tpoint.position.toQPointF(), m_dyntracker.getTrackingNode(b).position.toQPointF());
				}
			}

			//Draw node angle
			double len = radius / 1.8;

			PointF p1 = tpoint.position;
			p1.x += len * cos(tpoint.angle);
			p1.y += len * sin(tpoint.angle);

			PointF p2 = tpoint.position;
			p2.x -= len * cos(tpoint.angle);
			p2.y -= len * sin(tpoint.angle);

			m_painter->drawLine(p1.toQPointF(), p2.toQPointF());
		}
	}
}

void EDVSD_Anormaly_Detection::testEvents(EventF *p_buffer, int p_n)
{
	m_dyntracker.resetTracker();
	m_gngd.setLearning(false);
	cout << "Testing:" << endl;

	for(int a = 0; a < 40; a++){
		if(m_collectors[a] != NULL){
			delete m_collectors[a];
			m_collectors[a] = NULL;
		}
	}

	//Process all events of this frame, pass them to the tracker
	for(int a = 0; a < p_n; a++){
		//Skip "off" events //Todo
		if(p_buffer[a].polarity == false){
			continue;
		}

		m_dyntracker.analyzeEvent(p_buffer[a]);

		FeatureEvent event = m_dyntracker.popFeatureEvent();
		while(event.type != FEATURE_EVENT_TYPE_INVALID){
			double error = m_gngd.processFeatureEvent(event);

			switch(event.type){
			case FEATURE_EVENT_TYPE_NEW_NODE:
				m_collectors[event.id] = new ErrorCollector();
				m_collectors[event.id]->error += error;
				m_collectors[event.id]->n++;
				break;
			case FEATURE_EVENT_TYPE_LEARN_NODE:
				m_collectors[event.id]->error += error;
				m_collectors[event.id]->n++;
				break;
			case FEATURE_EVENT_TYPE_KILL_NODE:
				m_collectors[event.id]->error += error;
				m_collectors[event.id]->n++;

				cout << m_collectors[event.id]->error / m_collectors[event.id]->n << endl;

				delete m_collectors[event.id];
				m_collectors[event.id] = NULL;
				break;
			}

			event = m_dyntracker.popFeatureEvent();
		}
	}

}

void EDVSD_Anormaly_Detection::testLiveEvents(EventF *p_buffer, int p_n)
{
	//Process all events of this frame, pass them to the tracker
	for(int a = 0; a < p_n; a++){
		//Skip "off" events //Todo
		if(p_buffer[a].polarity == false){
			continue;
		}

		m_dyntracker.analyzeEvent(p_buffer[a]);
	}

	//double error = 0.0;

	FeatureEvent event = m_dyntracker.popFeatureEvent();
	while(event.type != FEATURE_EVENT_TYPE_INVALID){
		//double error = m_gngd.processFeatureEvent(event);
		event = m_dyntracker.popFeatureEvent();
	}

	//Visual output
	m_painter->fillRect(0,0,128,128,Qt::transparent);

	//m_painter->drawText(0,10, QString::number(error));

	for(int a = 0; a < m_dyntracker.getTrackerNum(); a++){
		if(m_dyntracker.isTrackingNodeActive(a)){
			TrackingNode tpoint = m_dyntracker.getTrackingNode(a);

			//Draw nodes
			m_painter->drawEllipse(tpoint.position.toQPointF(), 1.0, 1.0);

			//Draw node connections
			for(int b = 0; b < a; b++){
				double connection = m_dyntracker.getTrackingNodeConnection(a, b);
				if(connection > 0.8){
					m_painter->drawLine(tpoint.position.toQPointF(), m_dyntracker.getTrackingNode(b).position.toQPointF());
				}
			}

			//Draw node angle
			double len = 2.25;

			PointF p1 = tpoint.position;
			p1.x += len * cos(tpoint.angle);
			p1.y += len * sin(tpoint.angle);

			PointF p2 = tpoint.position;
			p2.x -= len * cos(tpoint.angle);
			p2.y -= len * sin(tpoint.angle);

			m_painter->drawLine(p1.toQPointF(), p2.toQPointF());
		}
	}
}
