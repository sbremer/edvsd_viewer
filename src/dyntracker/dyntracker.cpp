#include "dyntracker.h"

DynTracker::DynTracker()
	:m_attraction_fact(3.0), m_attraction_pow(3.0), m_attraction_max(0.3), m_initial_inf(0.03)
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
	list<TrackingUnit>::iterator trackermin;

	for(list<TrackingUnit>::iterator iter = m_trackers.begin(); iter != m_trackers.end(); iter++){
		double dist = p_event.position.getDistance(iter->point);
		if(dist < distmin){
			distmin = dist;
			trackermin = iter;
		}

		iter->age += 1.0;
	}

	if(distmin < 6.0){
		PointF delta = p_event.position - trackermin->point;

		double fact = m_attraction_fact / pow(PointF().getDistance(delta), m_attraction_pow);
		fact = min(m_attraction_max, fact);

		trackermin->point += delta * fact;

		trackermin->age /= 2.0;

		m_initial_ratio *= 0.999;
		if(m_initial_ratio < 1.0){
			m_initial_ratio = 1.0;
		}
	}
	else{
		m_initial_tracker = p_event.position * m_initial_inf + m_initial_tracker * (1.0 - m_initial_inf);

		m_initial_ratio *= 1.01;

		if(m_initial_ratio > 1.4){
			m_trackers.push_back(TrackingUnit(m_initial_tracker));
			m_initial_ratio = 1.0;
		}
	}

	for(list<TrackingUnit>::iterator iter = m_trackers.begin(); iter != m_trackers.end(); ){
		 if(iter->age > m_trackers.size() * 10){
			 iter = m_trackers.erase(iter);
		 }
		 else{
			 iter++;
		 }
	}
}
