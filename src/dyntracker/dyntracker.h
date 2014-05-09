#ifndef DYNTRACKER_H
#define DYNTRACKER_H

#include <list>
#include <vector>
#include <math.h>

#include "helper/eventf.h"
#include "helper/pointf.h"
#include "helper/feature_event.h"

using namespace std;

struct TrackingNode{
	TrackingNode(PointF p_point, unsigned int p_ts)
		:position(p_point), age(0.0), events(0), error(0.0), last(p_ts), rate(2000.0), angle(0.0), velocity(), looked_at(false)
	{

	}

	TrackingNode()
		:position(), age(0.0), events(0), error(0.0), last(0), rate(2000.0), angle(0.0), velocity(), looked_at(false)
	{

	}

	PointF position;
	double age;
	int events;
	double error;
	unsigned int last;
	double rate;
	double angle;
	PointF velocity;

	//For group searching/ ToDo: Replace with more intelligent algorithm
	bool looked_at;
};

class DynTracker
{
public:
    DynTracker();
	void analyzeEvent(EventF p_event);

	int getTrackerNum();
	TrackingNode getTrackingNode(int p_a);
	bool isTrackingNodeActive(int p_a);
	double getTrackingNodeConnection(int p_a, int p_b);

	int getFeatureNum();

	void resetTracker();

	FeatureEvent popFeatureEvent();

private:
	const int m_track_num;
	const int m_featurenum;
	int m_track_active;
	TrackingNode **m_track_trackingnodes;
	double **m_track_adj;

	void adjustTrackers(EventF p_event, int p_pointmin, double p_distmin, int p_pointmin2, double p_distmin2);
	void adjustInitial(EventF p_event);
	int createTrackingNode(PointF p_point, unsigned int p_ts);

	vector<double> buildFeatureVector(int p_node);

	PointF m_test_init[13*13];
	PointF m_test_init_move[13*13];
	double m_test_init_n[13*13];

	list<FeatureEvent> m_feature_events; //Todo: make accessible from outside
};

#endif // DYNTRACKER_H
