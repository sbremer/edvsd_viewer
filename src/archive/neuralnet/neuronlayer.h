#ifndef NEURONLAYER_H
#define NEURONLAYER_H

#include <vector>

using namespace std;

#include "neuron.h"

class NeuronLayer
{
public:
	NeuronLayer(int p_size, int p_size_beneath);
	NeuronLayer(int p_size, int p_size_beneath, ActivationFunction p_activation_function);
	void calculateOutput(const vector<double const*> &p_input);
	void calculateError(const vector<double const*> &p_exp_output);
	void calculateDelta(const vector<double*> &p_delta_ref);
	void initializeWeights(double p_rndabs, double p_rndabsbias);
	void updateWeights(const vector<const double *> &p_input, double p_learnrate);

	const vector<double const*> &getOutputRef();
	const vector<double*> &getDeltaRef();

	//Dummy, never used
	NeuronLayer& operator= (const NeuronLayer& p_value){
		return *this;
	}

private:
	vector<Neuron> m_neurons;
	vector<double const*> m_output;
	vector<double*> m_delta;
	const int m_size;
};

#endif // NEURONLAYER_H
