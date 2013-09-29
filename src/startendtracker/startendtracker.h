#ifndef STARTENDTRACKER_H
#define STARTENDTRACKER_H

#include <list>
#include <math.h>

#include "edvsd.h"
#include "helper/pointf.h"
#include "helper/motionf.h"

using namespace std;

class StartEndTracker
{
public:
	StartEndTracker();

	list<MotionF> trackPoints(EDVS_Event *p_buffer, int p_n);

private:
	list<MotionF> analyzeMotionStartpoints(EDVS_Event *p_buffer, int p_n);
	list<MotionF> analyzeMotionEndpoints(EDVS_Event *p_buffer, int p_n, list<MotionF> p_motions);

	//Parameter:
	const double m_attraction_fact_start = 2.5;
	const double m_attraction_pow_start = 3.2;
	const double m_dist = 1.0;
	const double m_attraction_fact_end = 2.0;
	const double m_attraction_pow_end = 3.3;
};

#endif // STARTENDTRACKER_H
