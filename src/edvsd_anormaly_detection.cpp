#include "edvsd_anormaly_detection.h"

#include <iostream>
using namespace std;

EDVSD_Anormaly_Detection::EDVSD_Anormaly_Detection(QObject *parent)
	:QObject(parent), m_dyntracker(), m_gngd(m_dyntracker.getFeatureNum())
{

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

}

void EDVSD_Anormaly_Detection::analyzeLiveEvents(EventF *p_buffer, int p_n)
{
	//Process all events of this frame, pass them to the tracker
	for(int a = 0; a < p_n; a++){
		//Skip "off" events //Todo
		if(p_buffer[a].polarity == false){
			continue;
		}

		m_dyntracker.analyzeEvent(p_buffer[a]);
	}


	//Visual output
	m_painter->fillRect(0,0,128,128,Qt::transparent);

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

void EDVSD_Anormaly_Detection::testEvents(EventF *p_buffer, int p_n)
{

}

void EDVSD_Anormaly_Detection::testLiveEvents(EventF *p_buffer, int p_n)
{
	m_painter->fillRect(0,0,128,128,Qt::transparent);
}
