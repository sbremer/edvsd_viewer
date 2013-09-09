#ifndef NEURALNET_H
#define NEURALNET_H

#include <vector>

#include <stdlib.h>
#include <time.h>

using namespace std;

#include "neuron.h"
#include "neuronlayer.h"

class NeuralNet
{
public:
	NeuralNet(int p_size, int p_layersize[], int p_inputsize);
	void initialize(double p_rndabs);
	double calculate(double p_input[]);
	double train(double p_input[], double p_exp_output[]);

private:
	const int m_size;
	const vector<int> m_layersize;
	const int m_inputsize;

	void calculateOutput(const vector<double const *> &p_input);
	void calculateDelta(const vector<double const *> &p_exp_output);
	void initializeWeights(double p_rndabs);
	void updateWeights(const vector<const double *> &p_input, double p_learnrate);

	vector<NeuronLayer> m_layers;
	vector<double> m_input;
	vector<double const*> m_input_ptr;

};

#endif // NEURALNET_H
