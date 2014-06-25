#include "dyntracker.h"

#include <iostream>
using namespace std;

DynTracker::DynTracker()
	:m_track_num(20), m_featurenum(10), m_connection_threshold(0.8)
{
	m_trackerage_initial = 50;

	//Todo: smaller grid?
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

	//Initiate Group list array
	m_groups = new list<int>*[m_track_num];

	for(int a = 0; a < m_track_num; a++){
		m_groups[a] = NULL;
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

int DynTracker::getFeatureNum()
{
	return m_featurenum;
}

FeatureEvent DynTracker::popFeatureEvent()
{
	FeatureEvent event;
	if(m_feature_events.empty()){
		event = FeatureEvent();
	}
	else{
		event = m_feature_events.front();
		m_feature_events.pop_front();
	}
	return event;
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

			//Remove "old" Tracking Points //Todo: Different deletion?
//			if(m_track_trackingnodes[a]->age > 5 * m_track_active + 20){
//				if(m_track_trackingnodes[a]->events > 6){
//					//Fire kill event to GNG
//					vector<double> features(2);
//					int at = 0;

//					features[at++] = m_track_trackingnodes[a]->position.x;
//					features[at++] = m_track_trackingnodes[a]->position.y;
//					m_feature_events.push_back(FeatureEvent(features, a, p_event.ts, FEATURE_EVENT_TYPE_KILL_NODE));
//				}

//				killTrackingNode(a);
//				continue;
//			}

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
			//m_track_trackingnodes[a]->age += 1.0;

		}
	}

	//Check for node close to input, Todo elipse (angle dependent) distance?
	if(pointmin != -1 && distmin < 9.0){
		adjustTrackers(p_event, pointmin, distmin, pointmin2, distmin2);

	}
	//Else run initializer algorithm
	else{
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

	//Override "old" fact calculation (not really useful I think)
	double fact = 0.15;

	//Execute attraction for closest trackingpoint
	closest->position += delta * fact;

	//Lower age of closest trackingpoint
	closest->age /= 3.0;

	//Increase event number for this node
	closest->events++;
	closest->events_slot++;

	//Update event angle
	double angle_imp = 0.06;
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

	//Apply simple distance error
	double error_imp = 0.05;
	closest->error = (1.0 - error_imp) * closest->error + error_imp * p_distmin * p_distmin;

	//Update event rate
	double rate_imp = 0.1;
	if(closest->events_slot == 10){
		unsigned int diff = p_event.ts - closest->last;
		closest->last = p_event.ts;
		closest->rate =  (1.0 - rate_imp) * closest->rate + rate_imp * (closest->events_slot / (0.000001 * diff));
		closest->events_slot = 0;
	}

	//Update tracker velocity
	double velocity_imp = 0.02;
	PointF velocity = delta * closest->rate;
	closest->velocity = closest->velocity * (1.0 - velocity_imp) + velocity * velocity_imp;

	//Attract all tracking points to the event depending on their connection strength to the closest event
	for(int a = 0; a < m_track_num; a++){
		if(p_pointmin == a || m_track_trackingnodes[a] == NULL){
			continue;
		}

		m_track_trackingnodes[a]->position += delta * fact * m_track_adj[max(p_pointmin, a)][min(p_pointmin, a)] * 0.4;
	}

	//Todo: different distance calculation elipsoid with angle and error

	//Check if 2nd closest trackingpoint is also nearby
	if(closest2 != NULL && p_distmin2 < 7.0){

		//Lower age
		closest2->age /= 1.5;

		//Strengthen the connection between closest and 2nd closest point
		double new_adj = min(1.0, m_track_adj[max(p_pointmin, p_pointmin2)][min(p_pointmin, p_pointmin2)] + 0.1);

		if(m_track_adj[max(p_pointmin, p_pointmin2)][min(p_pointmin, p_pointmin2)] < m_connection_threshold && new_adj >= m_connection_threshold){
			group(p_pointmin, p_pointmin2);
		}

		m_track_adj[max(p_pointmin, p_pointmin2)][min(p_pointmin, p_pointmin2)] = new_adj;
	}

	//Lower connection strength between closest trackingpoint to all other trackingpoints
	for(int a = 0; a < m_track_num; a++){
		if(p_pointmin == a || m_track_trackingnodes[a] == NULL){
			continue;
		}

		double new_adj = max(0.0, m_track_adj[max(p_pointmin, a)][min(p_pointmin, a)] - 0.01);

		if(m_track_adj[max(p_pointmin, a)][min(p_pointmin, a)] >= m_connection_threshold && new_adj < m_connection_threshold){
			ungroup(p_pointmin, a);
		}

		m_track_adj[max(p_pointmin, a)][min(p_pointmin, a)] = new_adj;

		//Age Trackers here
		m_track_trackingnodes[a]->age += 1.0;

		if(a != p_pointmin && a != p_pointmin2 && m_track_trackingnodes[a]->age > 6 * m_track_active + 10){
			if(m_track_trackingnodes[a]->events > m_trackerage_initial){
				//Fire kill event to GNG
				vector<double> features(2);
				int at = 0;

				features[at++] = m_track_trackingnodes[a]->position.x;
				features[at++] = m_track_trackingnodes[a]->position.y;
				m_feature_events.push_back(FeatureEvent(features, a, p_event.ts, FEATURE_EVENT_TYPE_KILL_NODE));
			}

			killTrackingNode(a);
			continue;
		}
	}

	//Check for a high error
	if(closest->error > 18.0){
		//Try to create a new point
		int new_track = newTrackingNode(closest->position, p_event.ts);
		if(new_track != -1){
			//Lower error of closest point if successful
			closest->error /= 2.0;

			//Connect new node to neighbor
			m_track_adj[max(p_pointmin, new_track)][min(p_pointmin, new_track)] = 1.0;
			group(p_pointmin, new_track);

			if(p_pointmin2 != -1){
				closest2->error /= 1.5;
				m_track_adj[max(p_pointmin2, new_track)][min(p_pointmin2, new_track)] = m_connection_threshold * 0.75;
			}
		}
	}

	//Build Feature Events for user class to call for
	if(closest->events == m_trackerage_initial){
		vector<double> features(2);
		int at = 0;

		features[at++] = closest->position.x;
		features[at++] = closest->position.y;

		if(closest->group != -1){
			m_feature_events.push_back(FeatureEvent(features, closest->group, p_event.ts, FEATURE_EVENT_TYPE_NEW_NODE));
		}
		else{
			m_feature_events.push_back(FeatureEvent(features, p_pointmin, p_event.ts, FEATURE_EVENT_TYPE_NEW_NODE));
		}
	}
	else if(closest->events > m_trackerage_initial){

		vector<double> features = buildFeatureVector(p_pointmin);
		if(closest->group != -1){
			m_feature_events.push_back(FeatureEvent(features, closest->group, p_event.ts, FEATURE_EVENT_TYPE_LEARN_NODE));
		}
		else{
			m_feature_events.push_back(FeatureEvent(features, p_pointmin, p_event.ts, FEATURE_EVENT_TYPE_LEARN_NODE));
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
		newTrackingNode(m_test_init_move[13 * x + y], p_event.ts);

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
		m_test_init_move[13 * x + y] += delta * 0.15;
	}
}

void DynTracker::group(int p_a, int p_b)
{
	//Verify that nodes are not connected already
	if(m_track_trackingnodes[p_a]->group != -1 && m_track_trackingnodes[p_a]->group == m_track_trackingnodes[p_b]->group){
		return;
	}

	if(m_track_trackingnodes[p_a]->group == -1 && m_track_trackingnodes[p_b]->group == -1){
		//New group number is min of both elements
		int group = min(p_a, p_b);

		//Update group info of nodes
		m_track_trackingnodes[p_a]->group = group;
		m_track_trackingnodes[p_b]->group = group;

		//Create group list
		m_groups[group] = new list<int>();
		m_groups[group]->push_back(p_a);
		m_groups[group]->push_back(p_b);
	}
	else if(m_track_trackingnodes[p_a]->group != -1 && m_track_trackingnodes[p_b]->group == -1){
		//Add p_b to p_a's group
		int group = m_track_trackingnodes[p_a]->group;
		m_track_trackingnodes[p_b]->group = group;
		m_groups[group]->push_back(p_b);

		//Ensure that the group index is the smallest node's index
		if(p_b < group){
			int ogroup = p_b;

			m_groups[ogroup] = new list<int>();

			for(list<int>::iterator iter = m_groups[group]->begin(); iter != m_groups[group]->end(); iter++){
				m_track_trackingnodes[*iter]->group = ogroup;
				m_groups[ogroup]->push_back(*iter);
			}

			//Delete group
			delete m_groups[group];
			m_groups[group] = NULL;
		}
	}
	else if(m_track_trackingnodes[p_a]->group == -1 && m_track_trackingnodes[p_b]->group != -1){
		//Add p_a to p_b's group
		int group = m_track_trackingnodes[p_b]->group;
		m_track_trackingnodes[p_a]->group = group;
		m_groups[group]->push_back(p_a);

		//Ensure that the group index is the smallest node's index
		if(p_a < group){
			int ogroup = p_a;

			m_groups[ogroup] = new list<int>();

			for(list<int>::iterator iter = m_groups[group]->begin(); iter != m_groups[group]->end(); iter++){
				m_track_trackingnodes[*iter]->group = ogroup;
				m_groups[ogroup]->push_back(*iter);
			}

			//Delete group
			delete m_groups[group];
			m_groups[group] = NULL;
		}
	}
	else{
		//Both are in groups already --> merge groups
		int group = min(m_track_trackingnodes[p_a]->group, m_track_trackingnodes[p_b]->group);
		int ogroup = max(m_track_trackingnodes[p_a]->group, m_track_trackingnodes[p_b]->group);

		//Add elements of ogroup to group
		for(list<int>::iterator iter = m_groups[ogroup]->begin(); iter != m_groups[ogroup]->end(); iter++){
			m_track_trackingnodes[*iter]->group = group;
			m_groups[group]->push_back(*iter);
		}

		//Delete ogroup
		delete m_groups[ogroup];
		m_groups[ogroup] = NULL;
	}
}

void DynTracker::ungroup(int p_a, int p_b)
{
	if(m_track_trackingnodes[p_a]->group == -1 || m_track_trackingnodes[p_b]->group == -1 || m_track_trackingnodes[p_a]->group != m_track_trackingnodes[p_b]->group){
		return;
	}

	int group = m_track_trackingnodes[p_a]->group;

	//Verify connection
	if(find(m_groups[group]->begin(), m_groups[group]->end(), p_b) == m_groups[group]->end()){
		return;
	}

	if(m_groups[group]->size() == 2){
		//Ungroup and delete group
		m_track_trackingnodes[p_a]->group = -1;
		m_track_trackingnodes[p_b]->group = -1;

		//Delete group
		delete m_groups[group];
		m_groups[group] = NULL;
	}
	else{
		//Group is larger than 2 nodes --> split groups if necessary
		list<int> group_list_a = findgroup(p_a);
		list<int> group_list_b = findgroup(p_b);

		if(find(group_list_a.begin(), group_list_a.end(), p_b) != group_list_a.end()){
			//Groups are still connected through some other node
			//Change nothing
		}
		else if(group_list_a.size() > 1 && group_list_b.size() == 1){
			//Split p_b off
			m_track_trackingnodes[p_b]->group = -1;
			m_groups[group]->remove(p_b);

			//Ensure that the group index is the smallest node's index
			if(group == p_b){
				int ogroup = m_track_num;

				for(list<int>::iterator iter = m_groups[group]->begin(); iter != m_groups[group]->end(); iter++){
					ogroup = min(ogroup, *iter);
				}

				m_groups[ogroup] = new list<int>();

				for(list<int>::iterator iter = m_groups[group]->begin(); iter != m_groups[group]->end(); iter++){
					m_track_trackingnodes[*iter]->group = ogroup;
					m_groups[ogroup]->push_back(*iter);
				}

				//Delete group
				delete m_groups[group];
				m_groups[group] = NULL;
			}
		}
		else if(group_list_a.size() == 1 && group_list_b.size() > 1){
			//Split p_a off
			m_track_trackingnodes[p_a]->group = -1;
			m_groups[group]->remove(p_a);

			//Ensure that the group index is the smallest node's index
			if(group == p_a){
				int ogroup = m_track_num;

				for(list<int>::iterator iter = m_groups[group]->begin(); iter != m_groups[group]->end(); iter++){
					ogroup = min(ogroup, *iter);
				}

				m_groups[ogroup] = new list<int>();

				for(list<int>::iterator iter = m_groups[group]->begin(); iter != m_groups[group]->end(); iter++){
					m_track_trackingnodes[*iter]->group = ogroup;
					m_groups[ogroup]->push_back(*iter);
				}

				//Delete group
				delete m_groups[group];
				m_groups[group] = NULL;
			}
		}
		else{
			//Both parts have more than one element

			//Delete group
			delete m_groups[group];
			m_groups[group] = NULL;

			//Find min value of new p_a group
			group = m_track_num;
			for(list<int>::iterator iter = group_list_a.begin(); iter != group_list_a.end(); iter++){
				group = min(group, *iter);
			}

			//Create new group
			m_groups[group] = new list<int>();

			//Add all nodes connected to p_a
			for(list<int>::iterator iter = group_list_a.begin(); iter != group_list_a.end(); iter++){
				m_track_trackingnodes[*iter]->group = group;
				m_groups[group]->push_back(*iter);
			}


			//Find min value of new p_b group
			group = m_track_num;
			for(list<int>::iterator iter = group_list_b.begin(); iter != group_list_b.end(); iter++){
				group = min(group, *iter);
			}

			//Create new group
			m_groups[group] = new list<int>();

			//Add all nodes connected to p_b
			for(list<int>::iterator iter = group_list_b.begin(); iter != group_list_b.end(); iter++){
				m_track_trackingnodes[*iter]->group = group;
				m_groups[group]->push_back(*iter);
			}
		}
	}
}

list<int> DynTracker::findgroup(int p_a)
{
	//Reset looked_at for all nodes
	for(int a = 0; a < m_track_num; a++){
		if(m_track_trackingnodes[a] != NULL){
			m_track_trackingnodes[a]->looked_at = false;
		}
	}

	list<int> group_list;
	list<int>::iterator iter;

	//Add first element
	group_list.push_back(p_a);
	m_track_trackingnodes[p_a]->looked_at = true;

	//Add all connected elements using BFS
	iter = group_list.begin();
	while(iter != group_list.end()){
		for(int a = 0; a < m_track_num; a++){
			if(m_track_trackingnodes[a] != NULL && a != *iter && !m_track_trackingnodes[a]->looked_at && m_track_adj[max(a, *iter)][min(a, *iter)] >= m_connection_threshold){
				group_list.push_back(a);
				m_track_trackingnodes[a]->looked_at = true;
			}
		}

		iter++;
	}

	return group_list;
}

int DynTracker::newTrackingNode(PointF p_point, unsigned int p_ts)
{
	//Search for free slot
	int free = -1;
	for(int a = 0; a < m_track_num; a++){
		if(m_track_trackingnodes[a] == NULL){
			free = a;
			break;
		}
	}
	if(free != -1){
		//Free slot found, initiate Tracker
		m_track_trackingnodes[free] = new TrackingNode(p_point, p_ts);

		//Increase counter
		m_track_active++;

		return free;
	}
	else{
		//TrackingNode buffer full!
		return -1;
	}
}

void DynTracker::killTrackingNode(int p_a)
{
	//Reset adjacency matrix for this node
	for(int a = 0; a < m_track_num; a++){
		if(p_a == a)continue;

		m_track_adj[max(p_a, a)][min(p_a, a)] = 0.0;
	}

	if(m_track_trackingnodes[p_a]->group != -1){

		//Node is in a group
		int group = m_track_trackingnodes[p_a]->group;
		list<int> group_list = *(m_groups[group]);

		//Delete group
		delete m_groups[group];
		m_groups[group] = NULL;

		for(list<int>::iterator iter = group_list.begin(); iter != group_list.end(); iter++){
			m_track_trackingnodes[*iter]->group = -1;
		}

		if(group_list.size() > 1){

			//Regroup nodes
			while(group_list.size() > 0){
				int i = group_list.front();
				group_list.pop_front();

				//Find group of element
				list<int> ogroup_list = findgroup(i);

				//Single Node
				if(ogroup_list.size() <= 1){
					m_track_trackingnodes[i]->group = -1;
				}
				//Multiple Nodes
				else{
					//Find min value of group
					int ogroup = m_track_num;
					for(list<int>::iterator iter = ogroup_list.begin(); iter != ogroup_list.end(); iter++){
						ogroup = min(ogroup, *iter);
						group_list.remove(*iter);
					}

					m_groups[ogroup] = new list<int>();

					//Add all nodes
					for(list<int>::iterator iter = ogroup_list.begin(); iter != ogroup_list.end(); iter++){
						m_track_trackingnodes[*iter]->group = ogroup;
						m_groups[ogroup]->push_back(*iter);
					}
				}
			}
		}
	}

	//Delete Node
	delete m_track_trackingnodes[p_a];
	m_track_trackingnodes[p_a] = NULL;

	//Decrease counter
	m_track_active--;
}

vector<double> DynTracker::buildFeatureVector(int p_a)
{
	PointF center = PointF();

	TrackingNode *node = m_track_trackingnodes[p_a];

	double error = 0;
	double rate = 0;

	int group = node->group;
	if(group != -1){

		//Find group center
		int n = 0;

		for(list<int>::iterator iter = m_groups[group]->begin(); iter != m_groups[group]->end(); iter++){
			center += m_track_trackingnodes[*iter]->position;
			n++;
			error += m_track_trackingnodes[*iter]->error;
			rate += m_track_trackingnodes[*iter]->rate;
		}

		center /= n;
	}
	else{
		center = node->position;
		error += node->error;
		rate += node->rate;
	}

	//Calculate distance to closest neighbor
	double min_dist_neighbor = INFINITY;

	for(int a = 0; a < m_track_num; a++){
		if(m_track_trackingnodes[a] == NULL || a == p_a || (m_track_trackingnodes[a]->group != -1 && m_track_trackingnodes[a]->group == group)){
			continue;
		}

		double dist = center.getDistance(m_track_trackingnodes[a]->position);
		if(dist < min_dist_neighbor){
			min_dist_neighbor = dist;
		}
	}

	if(min_dist_neighbor > 200.0){
		min_dist_neighbor = 0.0;
	}

	PointF center_node = node->position - center;

	//Build Feature Vector
	vector<double> features(m_featurenum);

	int at = 0;

	//An incrementing int allows easy exchange and changes in order of data
	//Values should be roughly in the area of [-1.0, 1.0]
	features[at++] = (center.x - 64.0) / 64.0;
	features[at++] = (center.y - 64.0) / 64.0;
	features[at++] = center_node.x / 5.0;
	features[at++] = center_node.y / 5.0;
	features[at++] = node->error / 14.0;
	features[at++] = node->rate / 1000.0; //Rate and velocity can be very different for different data sets!
	features[at++] = node->velocity.x / 1500.0;
	features[at++] = node->velocity.y / 1500.0;
	features[at++] = node->angle / M_PI_2;
	features[at++] = min_dist_neighbor / 15.0;
	//ToDo: More?

	return features;
}

void DynTracker::resetTracker()
{
	for(int a = 0; a < m_track_num; a++){

		//Remove Nodes
		if(m_track_trackingnodes[a] != NULL){
			delete m_track_trackingnodes[a];
			m_track_trackingnodes[a] = NULL;
		}

		//Remove Groups
		if(m_groups[a] != NULL){
			delete m_groups[a];
			m_groups[a] = NULL;
		}

		//Reset adjecency matrix
		for(int b = 0; b < a; b++){
			m_track_adj[a][b] = 0.0;
		}
	}

	m_feature_events.clear();

	m_track_active = 0;
}
