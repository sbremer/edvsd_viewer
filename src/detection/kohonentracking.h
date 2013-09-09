#ifndef KOHONENTRACKING_H
#define KOHONENTRACKING_H

#include <list>
#include <math.h>

using namespace std;

#include "particles.h"

template <int N>
class KohonenTracking
{
public:
	KohonenTracking();
	void initialize(PointF p_start, PointF p_end, bool p_trackpolarity);
	void analyzeEvent(PointF p_event, bool p_polarity, unsigned int p_ts);

private:
	const int m_size;
	list<Particle<N> > m_tracker;
	PointF m_start, m_end;
	bool m_trackpolarity;
	int m_spawn;
};

#endif // KOHONENTRACKING_H
