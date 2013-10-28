#ifndef GROWINGNEURALGAS_DRIVER_H
#define GROWINGNEURALGAS_DRIVER_H

#include <vector>

#include "growingneuralgas/growingneuralgas.h"
#include "output/gnuplot_output.h"

using namespace std;

class GrowingNeuralGas_Driver
{
public:
	GrowingNeuralGas_Driver(int p_dim);
	void learn(vector<double> p_input);
	double test(vector<double> p_input);
	void dumpData();

private:
	GrowingNeuralGas m_gng;
};

#endif // GROWINGNEURALGAS_DRIVER_H
