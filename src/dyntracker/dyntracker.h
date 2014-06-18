#ifndef DYNTRACKER_H
#define DYNTRACKER_H

#include <list>
#include <vector>
#include <algorithm>
#include <math.h>

#include "helper/eventf.h"
#include "helper/pointf.h"
#include "helper/feature_event.h"

using namespace std;

struct TrackingNode{
	TrackingNode(PointF p_point, unsigned int p_ts)
		:position(p_point), age(0.0), events(0), events_slot(0), error(0.0), last(p_ts), rate(2000.0), angle(0.0), velocity(), looked_at(false), group(-1), creation(p_ts)
	{

	}

	TrackingNode()
		:position(), age(0.0), events(0), error(0.0), last(0), rate(0.0), angle(0.0), velocity(), looked_at(false), group(-1)
	{

	}

	PointF position;
	double age;
	int events;
	int events_slot;
	double error;
	unsigned int last;
	double rate;
	double angle;
	PointF velocity;

	unsigned int creation;

	//For group searching/ ToDo: Replace with more intelligent algorithm
	bool looked_at;
	int group;
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
	const double m_connection_threshold;

	int m_trackerage_initial;

	int m_track_active;
	TrackingNode **m_track_trackingnodes;
	double **m_track_adj;

	list<int> **m_groups;
	void group(int p_a, int p_b);
	void ungroup(int p_a, int p_b);
	list<int> findgroup(int p_a);

	int newTrackingNode(PointF p_point, unsigned int p_ts);
	void killTrackingNode(int p_a);

	void adjustTrackers(EventF p_event, int p_pointmin, double p_distmin, int p_pointmin2, double p_distmin2);
	void adjustInitial(EventF p_event);


	vector<double> buildFeatureVector(int p_a);

	PointF m_test_init[13*13];
	PointF m_test_init_move[13*13];
	double m_test_init_n[13*13];

	list<FeatureEvent> m_feature_events; //Todo: make accessible from outside
};

#endif // DYNTRACKER_H
