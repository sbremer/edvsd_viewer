#ifndef KOHONENTRACKING_HELPER_H
#define KOHONENTRACKING_HELPER_H

#include "math.h"

inline double exp0_6(int p_dist)
{
	return pow(0.6, (double)p_dist);
}

typedef double (*NeighborFunction)(int);

const NeighborFunction Function_Exponential =  exp0_6;

#endif // KOHONENTRACKING_HELPER_H
