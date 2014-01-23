#ifndef DYNTRACKER_H
#define DYNTRACKER_H

#include <list>
#include <math.h>

#include "helper/eventf.h"
#include "helper/pointf.h"

using namespace std;

struct TrackingNode{
	TrackingNode(PointF p_point)
	{
		point = p_point;
	}

	PointF point;
	list<TrackingNode*> edges;
};

struct TrackingUnit{
	TrackingUnit()
	{
		point = PointF();
		age = 0.0;
		error = 20.0;
	}

	TrackingUnit(PointF p_point)
	{
		point = p_point;

	}

	void initialize(){
		nodes.push_back(TrackingNode(point));
		nodes.push_back(TrackingNode(point));
		list<TrackingNode>::iterator iter = nodes.begin();
		TrackingNode *node1 = &*iter;
		iter++;
		TrackingNode *node2 = &*iter;
		node1->edges.push_back(node2);
		node2->edges.push_back(node1);
		age = 0.0;
	}

	PointF point;
	list<TrackingNode> nodes;
	double age;
	double error;
};

class DynTracker
{
public:
    DynTracker();
	void analyzeEvent(EventF p_event);
	const list<TrackingUnit> &getTrackers();
	PointF getInitialTracker();

private:
	list<TrackingUnit> m_trackers;

	PointF m_initial_tracker;
	const double m_initial_inf;
	double m_initial_ratio;

	PointF m_test_init[13*13];
	PointF m_test_init_move[13*13];
	double m_test_init_n[13*13];
	bool check_ini(int px, int py, double ref);

	const double m_attraction_fact;
	const double m_attraction_pow;
	const double m_attraction_max;
};

#endif // DYNTRACKER_H
