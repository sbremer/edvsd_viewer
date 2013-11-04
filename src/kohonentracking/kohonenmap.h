#ifndef KOHONENMAP_H
#define KOHONENMAP_H

#include <vector>
#include "helper/pointf.h"

using namespace std;

template <int N>
struct KohonenMap{
	vector<PointF> points;
	int ts;
	double error;

	int events;
	double errorsum;

	KohonenMap(PointF p_point = PointF(), int p_ts = -1)
		:error(0.0), events(0), errorsum(0.0)
	{
		points.resize(N);
		for(int a=0;a<N;a++){
			points[a] = p_point;
		}
		ts = p_ts;
	}
};

#endif // KOHONENMAP_H
