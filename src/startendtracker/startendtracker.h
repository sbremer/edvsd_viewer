#ifndef STARTENDTRACKER_H
#define STARTENDTRACKER_H

#include <list>
#include <math.h>

#include "edvsd.h"
#include "helper/pointf.h"
#include "helper/motionf.h"
#include "helper/eventf.h"

using namespace std;

class StartEndTracker
{
public:
	StartEndTracker();
	StartEndTracker(double p_attraction_fact_start, double p_attraction_pow_start, double p_dist, double p_attraction_fact_end, double p_attraction_pow_end);

	list<MotionF> trackPoints(EDVS_Event *p_buffer, int p_n);
	list<MotionF> trackPoints(EventF *p_buffer, int p_n);

private:
	list<MotionF> analyzeMotionStartpoints(EDVS_Event *p_buffer, int p_n);
	list<MotionF> analyzeMotionEndpoints(EDVS_Event *p_buffer, int p_n, list<MotionF> p_motions);

	list<MotionF> analyzeMotionStartpoints(EventF *p_buffer, int p_n);
	list<MotionF> analyzeMotionEndpoints(EventF *p_buffer, int p_n, list<MotionF> p_motions);

	//Parameter:
	const double m_attraction_fact_start;
	const double m_attraction_pow_start;
	const double m_dist;
	const double m_attraction_fact_end;
	const double m_attraction_pow_end;
};

#endif // STARTENDTRACKER_H
