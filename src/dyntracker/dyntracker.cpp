#include "dyntracker.h"

#include <iostream>
using namespace std;

DynTracker::DynTracker()
	:m_attraction_fact(3.0), m_attraction_pow(2.0), m_attraction_max(0.3), m_initial_inf(0.05), m_track_num(60)
{
	m_initial_tracker = PointF(64.0, 64.0);
	m_initial_ratio = 1.0;

	for(int a = 0; a < 13; a++){
		for(int b = 0; b < 13; b++){
			m_test_init[a * 13 + b] = PointF(10 * a + 3, 10 * b + 3);
			m_test_init_move[a * 13 + b] = PointF(10 * a + 3, 10 * b + 3);
			m_test_init_n[a * 13 + b] = 0.0;
		}
	}

	//Adj Track; m_track_adj[>][<]
	m_track_trackingpoints = new TrackingPoint*[m_track_num];

	for(int a = 0; a < m_track_num; a++){
		m_track_trackingpoints[a] = NULL;
	}

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

const list<TrackingUnit> &DynTracker::getTrackers()
{
	return m_trackers;
}

PointF DynTracker::getInitialTracker()
{
	return m_initial_tracker;
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

int DynTracker::createTrackerPoint(PointF p_point)
{
	int free = -1;
	for(int a = 0; a < m_track_num; a++){
		if(m_track_trackingpoints[a] == NULL){
			free = a;
			break;
		}
	}
	if(free != -1){
		m_track_trackingpoints[free] = new TrackingPoint(p_point);

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
	double distmin = INFINITY;
	double distmin2 = INFINITY;
	int pointmin = -1;
	int pointmin2 = -1;

	for(int a = 0; a < m_track_num; a++){
		if(m_track_trackingpoints[a] != NULL){

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

			m_track_trackingpoints[a]->age += 1.0;

		}
	}

	if(pointmin != -1 && distmin < 6.0){ //Tracker found close to input
		PointF delta = p_event.position - m_track_trackingpoints[pointmin]->point;
		double fact = m_attraction_fact / pow(distmin, m_attraction_pow);
		fact = min(m_attraction_max, fact);

		m_track_trackingpoints[pointmin]->point += delta * fact;

		m_track_trackingpoints[pointmin]->age /= 2.0;

		for(int a = 0; a < m_track_num; a++){
			if(pointmin == a || m_track_trackingpoints[a] == NULL){
				continue;
			}

			m_track_adj[max(pointmin, a)][min(pointmin, a)] = max(0.0, m_track_adj[max(pointmin, a)][min(pointmin, a)] - 0.01);
		}

		double error_imp = 0.18;

		if(pointmin2 != -1 && distmin2 < 6.0){

			m_track_trackingpoints[pointmin2]->age /= 1.5;

			m_track_adj[max(pointmin, pointmin2)][min(pointmin, pointmin2)] = min(1.0, m_track_adj[max(pointmin, pointmin2)][min(pointmin, pointmin2)] + 0.15);

			//calculate orthogonal error
			PointF para = m_track_trackingpoints[pointmin]->point - m_track_trackingpoints[pointmin2]->point;
			PointF orth;
			orth.x = para.y;
			orth.y = para.x;
			double orthdist = (delta * orth) / delta.getAbs();
			if(delta.getAbs() == 0.0){
				orthdist = 0.0;
			}
//			orthdist = fabs(orthdist);

			double error = max(orthdist, distmin * 1.5);

			m_track_trackingpoints[pointmin]->error = (1.0 - error_imp) * m_track_trackingpoints[pointmin]->error + error_imp * error * error;
		}
		else{
			//apply abs error
			m_track_trackingpoints[pointmin]->error = (1.0 - error_imp) * m_track_trackingpoints[pointmin]->error + error_imp * distmin * distmin * 1.5;
		}

		if(m_track_trackingpoints[pointmin]->error > 23.0){
			int new_track = createTrackerPoint(m_track_trackingpoints[pointmin]->point);
			if(new_track != -1){
				m_track_trackingpoints[pointmin]->error = 0.0;
				m_track_adj[max(pointmin, new_track)][min(pointmin, new_track)] = 0.8;
				if(pointmin2 != -1){
					m_track_trackingpoints[pointmin2]->error = 0.0;
					m_track_adj[max(pointmin2, new_track)][min(pointmin2, new_track)] = 0.6;
				}
			}
		}

		for(int a = 0; a < m_track_num; a++){
			if(pointmin == a || m_track_trackingpoints[a] == NULL){
				continue;
			}

			m_track_trackingpoints[a]->point += delta * fact * m_track_adj[max(pointmin, a)][min(pointmin, a)] * 0.4;
		}

	}
	else{ //No input found close to a point
		int x = (2 + p_event.position.x) / 10;
		int y = (2 + p_event.position.y) / 10;

		if(m_test_init_move[13 * x + y].getDistance(m_test_init[13 * x + y]) > 3.0){
			createTrackerPoint(m_test_init_move[13 * x + y]);
			createTrackerPoint(m_test_init_move[13 * x + y]);
			m_test_init_move[13 * x + y] = m_test_init[13 * x + y];
		}
		else{
			PointF delta = p_event.position - m_test_init_move[13 * x + y];
			m_test_init_move[13 * x + y] += delta * 0.1;
		}
	}


	///////////////////////////////////////////////////////////////////////////////////////////
	return;

//	double distmin = INFINITY;
//	double distmin2 = INFINITY;
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

	if(trackermin == trackermin2 && distmin < 6.0){
		PointF para = nodemin->point - nodemin2->point;
		PointF delta = p_event.position - nodemin->point;
		PointF orth;
		orth.x = para.y;
		orth.y = para.x;
		double orthdist = (delta * orth) / delta.getAbs();
		if(delta.getAbs() == 0.0){
			orthdist = 0.0;
		}

		double errorrate = 0.15;
		trackermin->error = (1.0 - errorrate) * trackermin->error + errorrate * orthdist * orthdist;

		if(trackermin->error > 30.0){
			trackermin->nodes.push_back(TrackingNode((nodemin->point + nodemin2->point) * 0.5));
			trackermin->error = 0.0;
			nodemin->edges.remove(&*nodemin2);
			nodemin2->edges.remove(&*nodemin);
			nodemin->edges.push_back(&(trackermin->nodes.back()));
			nodemin2->edges.push_back(&(trackermin->nodes.back()));
			trackermin->nodes.back().edges.push_back(&*nodemin);
			trackermin->nodes.back().edges.push_back(&*nodemin2);
		}
		else{
			double fact = m_attraction_fact / pow(distmin, m_attraction_pow);
			fact = min(m_attraction_max, fact);

			PointF delta = p_event.position - nodemin->point;

			nodemin->point += delta * fact;
			for(list<TrackingNode*>::iterator iter = nodemin->edges.begin(); iter != nodemin->edges.end(); iter++){
				(*iter)->point += delta * fact * 0.5;
			}

			trackermin->age /= 2.0;
		}

//		cout << trackermin->error << endl;
	}

//	if(distmin < 10.0){
//		if(trackermin == trackermin2){
//			double fact = m_attraction_fact / pow(distmin, m_attraction_pow);
//			fact = min(m_attraction_max, fact);

//			PointF delta = p_event.position - nodemin->point;

//			nodemin->point += delta * fact;
//			for(list<TrackingNode*>::iterator iter = nodemin->edges.begin(); iter != nodemin->edges.end(); iter++){
//				(*iter)->point += delta * fact * 0.5;
//			}
//			//nodemin2->point += delta * fact * 0.2;
//		}

//		trackermin->age /= 2.0;

////		m_initial_ratio *= 0.999;
////		if(m_initial_ratio < 1.0){
////			m_initial_ratio = 1.0;
////		}
//	}
	else{
		int x = (2 + p_event.position.x) / 10;
		int y = (2 + p_event.position.y) / 10;
//		m_test_init_n[13 * x + y] += 1.0;
//		double ref = m_test_init_n[13 * x + y];

//		int checkval = 0;

//		if(check_ini(x-1, y-1, ref))checkval++;
//		if(check_ini(x-1, y, ref))checkval++;
//		if(check_ini(x-1, y+1, ref))checkval++;

//		if(check_ini(x, y-1, ref))checkval++;
//		if(check_ini(x, y+1, ref))checkval++;

//		if(check_ini(x+1, y-1, ref))checkval++;
//		if(check_ini(x+1, y, ref))checkval++;
//		if(check_ini(x+1, y+1, ref))checkval++;

//		if(checkval >= 7){
		if(m_test_init_move[13 * x + y].getDistance(m_test_init[13 * x + y]) > 3.0){
			m_trackers.push_back(TrackingUnit(m_test_init_move[13 * x + y]));
			m_trackers.back().initialize();
			m_test_init_n[13 * x + y] = 0.0;
			m_test_init_move[13 * x + y] = m_test_init[13 * x + y];
			for(int a = x - 1; a <= x + 1; a++){
				for(int b = y - 1; b <= y + 1; b++){
					if((a == x && b == y) || a < 0 || b < 0 || a > 12 || b > 12) continue;
					m_test_init_n[13 * a + b] /= 4.0;
				}
			}
		}
		else{
			PointF delta = p_event.position - m_test_init_move[13 * x + y];
			m_test_init_move[13 * x + y] += delta * 0.1;
		}


//		m_initial_tracker = p_event.position * m_initial_inf + m_initial_tracker * (1.0 - m_initial_inf);

//		m_initial_ratio *= 1.01;

//		if(m_initial_ratio > 1.3){
//			m_trackers.push_back(TrackingUnit(m_initial_tracker));
//			m_trackers.back().initialize();
//			m_initial_ratio = 1.0;
//		}
	}

	for(list<TrackingUnit>::iterator iter = m_trackers.begin(); iter != m_trackers.end(); ){
		 if(iter->age > m_trackers.size() * 8 + 5){
			 iter = m_trackers.erase(iter);
		 }
		 else{
			 iter++;
		 }
	}
}
