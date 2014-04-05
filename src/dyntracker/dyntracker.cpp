#include "dyntracker.h"

#include <iostream>
using namespace std;

DynTracker::DynTracker()
	:m_attraction_fact(3.0), m_attraction_pow(2.0), m_attraction_max(0.3), m_track_num(60)
{
	//Initiate point pattern for spawning Trackers (2D every 10px)
	for(int a = 0; a < 13; a++){
		for(int b = 0; b < 13; b++){
			m_test_init[a * 13 + b] = PointF(10 * a + 3, 10 * b + 3);
			m_test_init_move[a * 13 + b] = PointF(10 * a + 3, 10 * b + 3);
			m_test_init_n[a * 13 + b] = 0.0;
		}
	}

	//Initiate Tracking Points
	m_track_trackingpoints = new TrackingPoint*[m_track_num];

	for(int a = 0; a < m_track_num; a++){
		m_track_trackingpoints[a] = NULL;
	}

	//Initiate adjacency matrix, access with m_track_adj[>index][<index]
	m_track_adj = new double*[m_track_num];

	for(int a = 0; a < m_track_num; a++){
		if(a == 0){
			m_track_adj[a] = NULL;
		}
		else{
			m_track_adj[a] = new double[a];

			for(int b = 0; b < a; b++){
				m_track_adj[a][b] = 0.0;
			}
		}
	}

	m_track_active = 0;

}

int DynTracker::getTrackerNum()
{
	return m_track_num;
}

PointF DynTracker::getTracker(int p_a)
{
	if(m_track_trackingpoints[p_a] != NULL){
		return m_track_trackingpoints[p_a]->point;
	}

	return PointF();
}

bool DynTracker::isTrackerActive(int p_a)
{
	if(m_track_trackingpoints[p_a] != NULL){
		return true;
	}
	return false;
}

int DynTracker::createTrackerPoint(PointF p_point, unsigned int p_ts)
{
	//Search for free slot
	int free = -1;
	for(int a = 0; a < m_track_num; a++){
		if(m_track_trackingpoints[a] == NULL){
			free = a;
			break;
		}
	}
	if(free != -1){ //Free slot found, initiate Tracker
		m_track_trackingpoints[free] = new TrackingPoint(p_point, p_ts);

		for(int a = 0; a < m_track_num; a++){
			if(free == a)continue;

			m_track_adj[max(free, a)][min(free, a)] = 0.0;
		}
		m_track_active++;
		return free;
	}
	else{
		//TrackerPoint buffer full!
		return -1;
	}
}

void DynTracker::analyzeEvent(EventF p_event)
{
	//Search for the 2 Trackers closest to the input event
	double distmin = INFINITY;
	double distmin2 = INFINITY;
	int pointmin = -1;
	int pointmin2 = -1;

	for(int a = 0; a < m_track_num; a++){
		if(m_track_trackingpoints[a] != NULL){

			//Remove "old" Tracking Points
			if(m_track_trackingpoints[a]->age > 8 * m_track_active + 10){
				delete m_track_trackingpoints[a];
				m_track_trackingpoints[a] = NULL;
				m_track_active--;
				continue;
			}

			double dist = p_event.position.getDistance(m_track_trackingpoints[a]->point);
			if(dist < distmin){
				distmin2 = distmin;
				distmin = dist;
				pointmin2 = pointmin;
				pointmin = a;
			}
			else if(dist < distmin2){
				distmin2 = dist;
				pointmin2 = a;
			}

			//Age Tracking Points
			m_track_trackingpoints[a]->age += 1.0;

		}
	}

	if(pointmin != -1 && distmin < 6.0){ //Tracker found close to input
		adjustTrackers(p_event, pointmin, distmin, pointmin2, distmin2);

	}
	else{ //No input found close to a point, run initializer algorithm
		adjustInitial(p_event);
	}
}

void DynTracker::adjustTrackers(EventF p_event, int p_pointmin, double p_distmin, int p_pointmin2, double p_distmin2)
{
	PointF delta = p_event.position - m_track_trackingpoints[p_pointmin]->point;

	//Calculate attraction strength
	double fact = m_attraction_fact / pow(p_distmin, m_attraction_pow);
	fact = min(m_attraction_max, fact);

	//Execute attraction for closest trackingpoint
	m_track_trackingpoints[p_pointmin]->point += delta * fact;

	//Lower age of closest trackingpoint
	m_track_trackingpoints[p_pointmin]->age /= 3.0;

//	double rate_imp = 0.04;
//	unsigned int diff = p_event.ts - m_track_trackingpoints[p_pointmin]->last;
//	if(diff != 0){
//		m_track_trackingpoints[p_pointmin]->rate = (1.0 - rate_imp) * m_track_trackingpoints[p_pointmin]->rate + rate_imp * diff;
//	}
//	m_track_trackingpoints[p_pointmin]->last = p_event.ts;

	//Lower connection strength between closest trackingpoint to all other trackingpoints
	for(int a = 0; a < m_track_num; a++){
		if(p_pointmin == a || m_track_trackingpoints[a] == NULL){
			continue;
		}

		m_track_adj[max(p_pointmin, a)][min(p_pointmin, a)] = max(0.0, m_track_adj[max(p_pointmin, a)][min(p_pointmin, a)] - 0.01);
	}

	double error_imp = 0.18;

	//Check if 2nd closest trackingpoint is also nearby
	if(p_pointmin2 != -1 && p_distmin2 < 6.0){

		//Lower age
		m_track_trackingpoints[p_pointmin2]->age /= 1.5;

		//Strengthen the connection between closest and 2nd closest point
		m_track_adj[max(p_pointmin, p_pointmin2)][min(p_pointmin, p_pointmin2)] = min(1.0, m_track_adj[max(p_pointmin, p_pointmin2)][min(p_pointmin, p_pointmin2)] + 0.15);

		//calculate orthogonal error between the 2 points and the event
		PointF para = m_track_trackingpoints[p_pointmin]->point - m_track_trackingpoints[p_pointmin2]->point;
		PointF orth;
		orth.x = para.y;
		orth.y = para.x;
		double orthdist = (delta * orth) / delta.getAbs();
		if(delta.getAbs() == 0.0){
			orthdist = 0.0;
		}
//			orthdist = fabs(orthdist);

		double error = max(orthdist, p_distmin * 1.5);

		//Apply othogonal error
		m_track_trackingpoints[p_pointmin]->error = (1.0 - error_imp) * m_track_trackingpoints[p_pointmin]->error + error_imp * error * error;
	}
	else{
		//Apply simple distance error
		m_track_trackingpoints[p_pointmin]->error = (1.0 - error_imp) * m_track_trackingpoints[p_pointmin]->error + error_imp * p_distmin * p_distmin * 1.5;
	}

	//Check for a high error
	if(m_track_trackingpoints[p_pointmin]->error > 23.0){
		//Try to create a new point
		int new_track = createTrackerPoint(m_track_trackingpoints[p_pointmin]->point, p_event.ts);
		if(new_track != -1){
			//Lower error of closest point if successful
			m_track_trackingpoints[p_pointmin]->error = 0.0;
			m_track_adj[max(p_pointmin, new_track)][min(p_pointmin, new_track)] = 0.8;
			if(p_pointmin2 != -1){
				m_track_trackingpoints[p_pointmin2]->error = 0.0;
				m_track_adj[max(p_pointmin2, new_track)][min(p_pointmin2, new_track)] = 0.6;
			}
		}
	}

	//Attract all tracking points to the event depending on their connection strength to the closest event
	for(int a = 0; a < m_track_num; a++){
		if(p_pointmin == a || m_track_trackingpoints[a] == NULL){
			continue;
		}

		m_track_trackingpoints[a]->point += delta * fact * m_track_adj[max(p_pointmin, a)][min(p_pointmin, a)] * 0.4;
	}
}

void DynTracker::adjustInitial(EventF p_event)
{
	//Find closest initializerpoint
	int x = (2 + p_event.position.x) / 10;
	int y = (2 + p_event.position.y) / 10;

	//Check if initializerpoint is far away from origin
	if(m_test_init_move[13 * x + y].getDistance(m_test_init[13 * x + y]) > 3.0){
		//Spawn new Tracker
		createTrackerPoint(m_test_init_move[13 * x + y], p_event.ts);

		//Reset initializerpoint's position
		m_test_init_move[13 * x + y] = m_test_init[13 * x + y];
	}
	else{
		//Pull closest initializerpoint towards event
		PointF delta = p_event.position - m_test_init_move[13 * x + y];
		m_test_init_move[13 * x + y] += delta * 0.1;
	}
}
