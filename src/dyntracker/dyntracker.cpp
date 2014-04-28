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
	m_track_trackingnodes = new TrackingNode*[m_track_num];

	for(int a = 0; a < m_track_num; a++){
		m_track_trackingnodes[a] = NULL;
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

TrackingNode DynTracker::getTrackingNode(int p_a)
{
	if(m_track_trackingnodes[p_a] != NULL){
		return *(m_track_trackingnodes[p_a]);
	}

	return TrackingNode();
}

bool DynTracker::isTrackingNodeActive(int p_a)
{
	if(m_track_trackingnodes[p_a] != NULL){
		return true;
	}
	return false;
}

double DynTracker::getTrackingNodeConnection(int p_a, int p_b)
{
	if(m_track_trackingnodes[p_a] == NULL || m_track_trackingnodes[p_b] == NULL){
		return 0.0;
	}

	return m_track_adj[max(p_a, p_b)][min(p_a, p_b)];
}

int DynTracker::createTrackingNode(PointF p_point, unsigned int p_ts)
{
	//Search for free slot
	int free = -1;
	for(int a = 0; a < m_track_num; a++){
		if(m_track_trackingnodes[a] == NULL){
			free = a;
			break;
		}
	}
	if(free != -1){ //Free slot found, initiate Tracker
		m_track_trackingnodes[free] = new TrackingNode(p_point, p_ts);

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
		if(m_track_trackingnodes[a] != NULL){

			//Remove "old" Tracking Points
			if(m_track_trackingnodes[a]->age > 5 * m_track_active + 10){
				delete m_track_trackingnodes[a];
				m_track_trackingnodes[a] = NULL;
				m_track_active--;
				continue;
			}

			double dist = p_event.position.getDistance(m_track_trackingnodes[a]->position);
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
			m_track_trackingnodes[a]->age += 1.0;

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
	TrackingNode *closest = m_track_trackingnodes[p_pointmin];

	TrackingNode *closest2;
	if(p_pointmin2 == -1){
		closest2 = NULL;
	}
	else{
		closest2 = m_track_trackingnodes[p_pointmin2];
	}

	PointF delta = p_event.position - closest->position;

	//Calculate attraction strength
	double fact = m_attraction_fact / pow(p_distmin, m_attraction_pow);
	fact = min(m_attraction_max, fact);

	//Override "old" fact calculation (not really useful I think)
	fact = 0.15;

	//Execute attraction for closest trackingpoint
	closest->position += delta * fact;

	//Lower age of closest trackingpoint
	closest->age /= 3.0;

	//Increase event number for this node
	closest->events++;

	//Check if node has passed initial adjustment process
	if(closest->events == 5){
		//ToDo: Fire NowItem to GNG
	}

	//Update event rate
	double rate_imp = 0.04;
	unsigned int diff = p_event.ts - closest->last;
	if(diff != 0){
		closest->rate = (1.0 - rate_imp) * closest->rate + rate_imp * diff;
	}
	else{
		diff = 1;
	}
	closest->last = p_event.ts;

	//Update event angle
	double angle_imp = 0.05;
	double angle = atan(delta.y / delta.x);
	if(fabs(angle - closest->angle) > M_PI_2){
		if(angle > closest->angle){
			angle -= M_PI;
		}
		else{
			angle += M_PI;
		}
	}

	closest->angle = (1.0 - angle_imp) * closest->angle + angle_imp * angle;

	closest->angle = fmod(closest->angle + M_PI_2, M_PI) - M_PI_2;

	//Update tracker velocity
	double velocity_imp = 0.1;
	closest->velocity = closest->velocity * (1.0 - velocity_imp) + delta * velocity_imp * 1000000.0 / (double)diff;

	//Attract all tracking points to the event depending on their connection strength to the closest event
	for(int a = 0; a < m_track_num; a++){
		if(p_pointmin == a || m_track_trackingnodes[a] == NULL){
			continue;
		}

		m_track_trackingnodes[a]->position += delta * fact * m_track_adj[max(p_pointmin, a)][min(p_pointmin, a)] * 0.4;
	}

	//Lower connection strength between closest trackingpoint to all other trackingpoints
	for(int a = 0; a < m_track_num; a++){
		if(p_pointmin == a || m_track_trackingnodes[a] == NULL){
			continue;
		}

		m_track_adj[max(p_pointmin, a)][min(p_pointmin, a)] = max(0.0, m_track_adj[max(p_pointmin, a)][min(p_pointmin, a)] - 0.01);
	}

	double error_imp = 0.18;

	//Check if 2nd closest trackingpoint is also nearby
	if(closest2 != NULL && p_distmin2 < 6.0){

		//Lower age
		closest2->age /= 1.5;

		//Strengthen the connection between closest and 2nd closest point
		m_track_adj[max(p_pointmin, p_pointmin2)][min(p_pointmin, p_pointmin2)] = min(1.0, m_track_adj[max(p_pointmin, p_pointmin2)][min(p_pointmin, p_pointmin2)] + 0.15);

		//Update angle
//		closest2->angle = (1.0 - angle_imp * 0.4) * closest2->angle + angle_imp * 0.4 * angle;
//		closest2->angle = fmod(closest2->angle + M_PI_2, M_PI) - M_PI_2;

		//calculate orthogonal error between the 2 points and the event
		PointF para = closest->position - closest2->position;
		PointF orth;
		orth.x = para.y;
		orth.y = para.x;
		double orthdist = (delta * orth) / delta.getAbs();
		if(delta.getAbs() == 0.0){
			orthdist = 0.0;
		}
//			orthdist = fabs(orthdist);

		double error = min(orthdist, p_distmin);

		//Override orth. error
		error = p_distmin;

		//Apply othogonal error
		closest->error = (1.0 - error_imp) * closest->error + error_imp * error * error;
	}
	else{
		//Apply simple distance error
		closest->error = (1.0 - error_imp) * closest->error + error_imp * p_distmin * p_distmin;
	}

	//Check for a high error
	if(closest->error > 14.0){
		//Try to create a new point
		int new_track = createTrackingNode(closest->position, p_event.ts);
		if(new_track != -1){
			//Lower error of closest point if successful
			closest->error = 0.0;
			m_track_adj[max(p_pointmin, new_track)][min(p_pointmin, new_track)] = 0.8;
			if(p_pointmin2 != -1){
				closest2->error = 0.0;
				m_track_adj[max(p_pointmin2, new_track)][min(p_pointmin2, new_track)] = 0.6;
			}
		}
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
		createTrackingNode(m_test_init_move[13 * x + y], p_event.ts);

		//Reset initializerpoint's position
		m_test_init_move[13 * x + y] = m_test_init[13 * x + y];

		//(Half-) Reset surrounding initialtracker's position
		for(int a = x - 1; a <= x + 1; a++){
			for(int b = y - 1; b <= y + 1; b++){
				if( (a == x && b == y) || a < 0 || a > 12 || b < 0 || b > 12){
					continue;
				}

				m_test_init_move[13 * a + b] = m_test_init_move[13 * a + b] * 0.5 + m_test_init[13 * a + b] * 0.5;
			}
		}
	}
	else{
		//Pull closest initializerpoint towards event
		PointF delta = p_event.position - m_test_init_move[13 * x + y];
		m_test_init_move[13 * x + y] += delta * 0.1;
	}
}
