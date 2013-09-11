#ifndef NEURON_H
#define NEURON_H

#include <vector>

#include <stdlib.h>

using namespace std;

#include "neuralnet_functions.h"

class Neuron
{
public:
	Neuron(int p_size);
	Neuron();
	void calculateOutput(const vector<double const*> &p_input);
	void calculateDelta(const vector<double *> &p_delta_ref);
	void initializeWeights(double p_rndabs);
	void updateWeights(const vector<const double *> &p_input, double p_learnrate);

	//Dummy, never used
	Neuron& operator= (const Neuron& p_value){
		return *this;
	}

	double const * const getOutputRef();
	double m_delta;

private:
	vector<double> m_weights;
	const int m_size;
	double m_net;
	double m_bias;
	double m_output;
	const ActivationFunction m_activation_function;

	static double generateRandom(double p_rndabs);
};

#endif // NEURON_H
