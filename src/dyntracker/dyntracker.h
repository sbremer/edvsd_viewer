#ifndef DYNTRACKER_H
#define DYNTRACKER_H

#include <list>
#include <math.h>

#include "helper/eventf.h"
#include "helper/pointf.h"

using namespace std;

struct TrackingPoint{
	TrackingPoint(PointF p_point, unsigned int p_ts)
		:point(p_point), age(0.0), error(0.0), last(p_ts), rate(2000.0)
	{

	}

	PointF point;
	double age;
	double error;
	unsigned int last;
	double rate;
};

class DynTracker
{
public:
    DynTracker();
	void analyzeEvent(EventF p_event);

	int getTrackerNum();
	PointF getTracker(int p_a);
	bool isTrackerActive(int p_a);

private:
	const int m_track_num;
	int m_track_active;
	TrackingPoint **m_track_trackingpoints;
	double **m_track_adj;

	void adjustTrackers(EventF p_event, int p_pointmin, double p_distmin, int p_pointmin2, double p_distmin2);
	void adjustInitial(EventF p_event);
	int createTrackerPoint(PointF p_point, unsigned int p_ts);

	PointF m_test_init[13*13];
	PointF m_test_init_move[13*13];
	double m_test_init_n[13*13];

	const double m_attraction_fact;
	const double m_attraction_pow;
	const double m_attraction_max;
};

#endif // DYNTRACKER_H
