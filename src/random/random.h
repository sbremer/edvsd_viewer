#ifndef RANDOM_H
#define RANDOM_H

#include <stdlib.h>
#include <time.h>

class Random
{
public:
	Random();
	double randomDouble(double p_min, double p_max);
};

#endif // RANDOM_H
