#ifndef MOTIONF_H
#define MOTIONF_H

#include "pointf.h"

struct MotionF{
	PointF start,end;
	int num;

	bool operator==(MotionF& a) const{
		return (a.start.x==start.x && a.start.y==start.y);
	}
};

#endif // MOTIONF_H
