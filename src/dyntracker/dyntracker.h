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
	}

	TrackingUnit(PointF p_point)
	{
		point = p_point;
		nodes.push_back(TrackingNode(point));
		nodes.push_back(TrackingNode(point));
		age = 0.0;
	}

	PointF point;
	list<TrackingNode> nodes;
	double age;
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

	const double m_attraction_fact;
	const double m_attraction_pow;
	const double m_attraction_max;
};

#endif // DYNTRACKER_H
