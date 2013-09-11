#ifndef KOHONENTRACKING_H
#define KOHONENTRACKING_H

#include <list>
#include <math.h>

using namespace std;

#include "pointf.h"
#include "kohonenmap.h"
#include "kohonentracking_helper.h"

template <int N>
class KohonenTracking
{
public:
	KohonenTracking();
	void initialize(PointF p_start, PointF p_end, bool p_trackpolarity);
	const KohonenMap<N> *analyzeEvent(PointF p_event, bool p_polarity, unsigned int p_ts);
	const KohonenMap<N> *getKohonenMap(int p_at);
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
};

#endif // KOHONENTRACKING_H
