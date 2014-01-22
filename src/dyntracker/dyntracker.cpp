#include "dyntracker.h"

#include <iostream>
using namespace std;

DynTracker::DynTracker()
	:m_attraction_fact(3.0), m_attraction_pow(2.0), m_attraction_max(0.3), m_initial_inf(0.05)
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
}

const list<TrackingUnit> &DynTracker::getTrackers()
{
	return m_trackers;
}

PointF DynTracker::getInitialTracker()
{
	return m_initial_tracker;
}

bool DynTracker::check_ini(int px, int py, double ref)
{
   if(px >= 0 && px < 13 && py >= 0 && py < 13){
	   if(m_test_init_n[13 * px + py] < ref - 5){
		   return true;
	   }
   }
   else{
	   return true;
   }
   return false;
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

	if(trackermin == trackermin2 && distmin < 20.0){
		PointF para = nodemin->point - nodemin2->point;
		PointF delta = p_event.position - nodemin->point;
		PointF orth;
		orth.x = para.y;
		orth.y = para.x;
		double orthdist = (delta * orth) / delta.getAbs();

		double errorrate = 0.2;
		trackermin->error = (1.0 - errorrate) * trackermin->error + errorrate * orthdist * orthdist;

		if(trackermin->error > 25.0){
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

		}

		cout << trackermin->error << endl;
	}

	if(distmin < 5.0){
		if(trackermin == trackermin2){
			double fact = m_attraction_fact / pow(distmin, m_attraction_pow);
			fact = min(m_attraction_max, fact);

			PointF delta = p_event.position - nodemin->point;

			nodemin->point += delta * fact;
			for(list<TrackingNode*>::iterator iter = nodemin->edges.begin(); iter != nodemin->edges.end(); iter++){
				(*iter)->point += delta * fact * 0.2;
			}
			//nodemin2->point += delta * fact * 0.2;
		}

		trackermin->age /= 2.0;

		m_initial_ratio *= 0.999;
		if(m_initial_ratio < 1.0){
			m_initial_ratio = 1.0;
		}
	}
	else{
		int x = (2 + p_event.position.x) / 10;
		int y = (2 + p_event.position.y) / 10;
		m_test_init_n[13 * x + y] += 1.0;
		double ref = m_test_init_n[13 * x + y];

		int checkval = 0;

		if(check_ini(x-1, y-1, ref))checkval++;
		if(check_ini(x-1, y, ref))checkval++;
		if(check_ini(x-1, y+1, ref))checkval++;

		if(check_ini(x, y-1, ref))checkval++;
		if(check_ini(x, y+1, ref))checkval++;

		if(check_ini(x+1, y-1, ref))checkval++;
		if(check_ini(x+1, y, ref))checkval++;
		if(check_ini(x+1, y+1, ref))checkval++;

		if(checkval >= 7){
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
		 if(iter->age > m_trackers.size() * 8){
			 iter = m_trackers.erase(iter);
		 }
		 else{
			 iter++;
		 }
	}
}
