#ifndef EVENTF_H
#define EVENTF_H

#include "pointf.h"

struct EventF{
	EventF()
	{

	}

	EventF(char p_x, char p_y, char p_p, unsigned int p_ts)
	{
		position = PointF((double)p_x, (double)p_y);
		polarity = (bool)p_p;
		ts = p_ts;
	}

	PointF position;
	bool polarity;
	unsigned int ts;
};

#endif // EVENTF_H
