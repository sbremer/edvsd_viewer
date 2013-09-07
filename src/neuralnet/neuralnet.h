#ifndef NEURALNET_H
#define NEURALNET_H

#include <vector>

using namespace std;

#include "neuron.h"
#include "neuronlayer.h"

class NeuralNet
{
public:
	NeuralNet(int p_size, int p_layersize[], int p_inputsize);
	double calculate(double p_input[]);

private:
	const int m_size;
	const vector<int> m_layersize;
	const int m_inputsize;

	void calculateOutput(const vector<double const *> &p_input);

	vector<NeuronLayer> m_layers;
	vector<double> m_input;
	vector<double const*> m_input_ptr;

};

#endif // NEURALNET_H
