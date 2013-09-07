#ifndef NEURONLAYER_H
#define NEURONLAYER_H

#include <vector>

using namespace std;

#include "neuron.h"

class NeuronLayer
{
public:
	NeuronLayer(int p_size, int p_size_beneath);
	NeuronLayer();
	void calculateOutput(const vector<double const*> &p_input);

	const vector<double const*> &m_output_ref;

	//Dummy, never used
	NeuronLayer& operator= (const NeuronLayer& p_value){
		return *this;
	}

private:
	vector<Neuron> m_neurons;
	vector<double const*> m_output;
	const int m_size;
};

#endif // NEURONLAYER_H
