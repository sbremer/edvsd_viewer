#ifndef KOHONENTRACKING_H
#define KOHONENTRACKING_H

#include <list>
#include <math.h>

using namespace std;

#include "helper/pointf.h"
#include "helper/eventf.h"
#include "kohonenmap.h"
#include "kohonentracking_helper.h"

template <int N>
class KohonenTracking
{
public:
	KohonenTracking();
	KohonenTracking(double p_start_dist, double p_end_dist, double p_attraction_fact, double p_attraction_pow, double p_attraction_max, double p_neighbor_attraction);
	void initialize(PointF p_start, PointF p_end, bool p_trackpolarity);
	//KohonenMap<N> *analyzeEvent(PointF p_event, bool p_polarity, unsigned int p_ts);
	KohonenMap<N> *analyzeEvent(EventF p_event);
	KohonenMap<N> *getKohonenMap(int p_at);
	double getTrackerDistance(const KohonenMap<N> * p_tracker);
	int getDurationMin();
	int getListLength();

	const int m_size;

private:
	const NeighborFunction m_function;
	int m_neighbors;

	list<KohonenMap<N> > m_tracker;
	PointF m_start, m_end;

	typename list<KohonenMap<N> >::iterator m_iterator;
	int m_iterator_at;

	bool m_trackpolarity;
	int m_spawn;
	int m_duration_min;

	//Parameters:
	const double m_start_dist;
	const double m_end_dist;
	const double m_attraction_fact;
	const double m_attraction_pow;
	const double m_attraction_max;
	const double m_neighbor_attraction;
};

#endif // KOHONENTRACKING_H
