#ifndef EVENTF_H
#define EVENTF_H

#include "pointf.h"

struct EventF{
	PointF position;
	bool polarity;
	unsigned int ts;
};

#endif // EVENTF_H
