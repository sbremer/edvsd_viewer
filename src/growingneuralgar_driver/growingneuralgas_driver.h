#ifndef GROWINGNEURALGAS_DRIVER_H
#define GROWINGNEURALGAS_DRIVER_H

#include <vector>

#include "growingneuralgas/growingneuralgas.h"
#include "output/gnuplot_output.h"
#include "helper/feature_event.h"
#include "normalizer/normalizer.h"

using namespace std;

class GrowingNeuralGas_Driver
{
public:
	GrowingNeuralGas_Driver(int p_dim);
	double processFeatureEvent(FeatureEvent p_featureevent);

	double learn_node(vector<double> p_input, int p_id, unsigned int p_time);
	double new_node(vector<double> p_input, int p_id, unsigned int p_time);
	double kill_node(vector<double> p_input, int p_id, unsigned int p_time);

	void setLearning(bool p_learning);

	void dumpData();

private:
	bool m_learning;
	GrowingNeuralGas m_gng;
	Normalizer m_normalizer;
};

#endif // GROWINGNEURALGAS_DRIVER_H
