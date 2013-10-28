#include "random.h"

Random::Random()
{
	srand(time(0));
}

double Random::randomDouble(double p_min, double p_max)
{
	return (rand()%1000000) / 1000000.0 * (p_max - p_min) + p_min;
}
