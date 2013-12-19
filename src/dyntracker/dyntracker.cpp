#include "dyntracker.h"

#include <iostream>
using namespace std;

DynTracker::DynTracker()
	:m_attraction_fact(3.0), m_attraction_pow(3.0), m_attraction_max(0.3), m_initial_inf(0.05)
{
	m_initial_tracker = PointF(64.0, 64.0);
	m_initial_ratio = 1.0;
}

const list<TrackingUnit> &DynTracker::getTrackers()
{
	return m_trackers;
}

PointF DynTracker::getInitialTracker()
{
	return m_initial_tracker;
}

void DynTracker::analyzeEvent(EventF p_event)
{
	double distmin = INFINITY;
	double distmin2 = INFINITY;
	list<TrackingUnit>::iterator trackermin;
	list<TrackingNode>::iterator nodemin;
	list<TrackingUnit>::iterator trackermin2;
	list<TrackingNode>::iterator nodemin2;

	for(list<TrackingUnit>::iterator iter = m_trackers.begin(); iter != m_trackers.end(); iter++){
		for(list<TrackingNode>::iterator jter = iter->nodes.begin(); jter != iter->nodes.end(); jter++){
			double dist = p_event.position.getDistance(jter->point);
			if(dist < distmin){
				distmin2 = distmin;
				trackermin2 = trackermin;
				nodemin2 = nodemin;
				distmin = dist;
				trackermin = iter;
				nodemin = jter;
			}
			else if(dist < distmin2){
				distmin2 = dist;
				trackermin2 = iter;
				nodemin2 = jter;
			}
		}

		iter->age += 1.0;
	}

	if(distmin < 8.0){
		if(trackermin == trackermin2){
			//cout << "Point " << nodemin->point.x << " " << nodemin->point.y << endl;
			double gamma = atan2(p_event.position.x - nodemin->point.x, p_event.position.y - nodemin->point.y)
					- atan2(nodemin2->point.x - nodemin->point.x, nodemin2->point.y - nodemin->point.y);
			double h = sin(gamma) * distmin;
			PointF delta = PointF(nodemin2->point.y - nodemin->point.y, nodemin2->point.x - nodemin->point.x);
			double dist = PointF().getDistance(delta);
			delta *= h / dist;
			if(dist < 0.01){
				delta = PointF();
				cout << "over" << endl;
			}
			double fact = m_attraction_fact / pow(distmin, m_attraction_pow);
			fact = min(m_attraction_max, fact);
			cout << delta << endl;
			nodemin->point += delta * 0.2;
			nodemin2->point += delta * 0.2;

			nodemin->point += (p_event.position - nodemin->point) * fact * 0.1;
		}
		else{
			PointF delta = p_event.position - nodemin->point;

			double fact = m_attraction_fact / pow(PointF().getDistance(delta), m_attraction_pow);
			fact = min(m_attraction_max, fact);

			nodemin->point += delta * fact;
		}

		trackermin->age /= 2.0;

		m_initial_ratio *= 0.999;
		if(m_initial_ratio < 1.0){
			m_initial_ratio = 1.0;
		}
	}
	else{
		m_initial_tracker = p_event.position * m_initial_inf + m_initial_tracker * (1.0 - m_initial_inf);

		m_initial_ratio *= 1.01;

		if(m_initial_ratio > 1.3){
			m_trackers.push_back(TrackingUnit(m_initial_tracker));
			m_initial_ratio = 1.0;
		}
	}

	for(list<TrackingUnit>::iterator iter = m_trackers.begin(); iter != m_trackers.end(); ){
		 if(iter->age > m_trackers.size() * 8){
			 iter = m_trackers.erase(iter);
		 }
		 else{
			 iter++;
		 }
	}
}
