#include "neuralnet.h"

//For Debugging
#include "iostream"
using namespace std;

NeuralNet::NeuralNet(int p_size, int p_layersize[], int p_inputsize)
	:m_size(p_size), m_layersize(p_layersize, p_layersize + p_size), m_inputsize(p_inputsize)
{
	srand(time(0));

	m_layers.reserve(m_size);
	m_input.resize(m_inputsize);
	m_input_ptr.resize(m_inputsize);

	for(int a = 0; a < m_inputsize; a++){
		m_input_ptr[a] = &(m_input[a]);
	}

	for(int a = 0; a < m_size; a++){
		if(a == 0)
			m_layers.push_back(NeuronLayer(m_layersize[a], m_inputsize));
		else
			m_layers.push_back(NeuronLayer(m_layersize[a], m_layersize[a-1]));
	}
}

void NeuralNet::initialize(double p_rndabs)
{
	initializeWeights(p_rndabs);

	for(int a = 0; a < m_inputsize; a++){
		m_input_ptr[a] = &(m_input[a]);
	}
}

double NeuralNet::calculate(double p_input[])
{
	m_input.assign(p_input, p_input + m_inputsize);

	calculateOutput(m_input_ptr);

	return *(m_layers[m_size-1].getOutputRef()[0]);
}

double NeuralNet::train(double p_input[], double p_exp_output[])
{
	m_input.assign(p_input, p_input + m_inputsize);

	calculateOutput(m_input_ptr);

	vector<double const*> exp_output;
	exp_output.resize(m_layersize[m_size-1]);

	for(int a = 0; a < m_layersize[m_size-1]; a++){
		exp_output[a] = &(p_exp_output[a]);
	}

	calculateDelta(exp_output);

	updateWeights(m_input_ptr, 0.1);
}

void NeuralNet::calculateOutput(const vector<const double *> &p_input)
{
	m_layers[0].calculateOutput(p_input);

	for(int a = 1; a < m_size; a++){
		m_layers[a].calculateOutput(m_layers[a-1].getOutputRef());
	}
}

void NeuralNet::calculateDelta(const vector<const double *> &p_exp_output)
{
	m_layers[m_size-1].calculateError(p_exp_output);

	for(int a = m_size - 1; a > 0; a--){
		m_layers[a].calculateDelta(m_layers[a-1].getDeltaRef());
	}
}

void NeuralNet::initializeWeights(double p_rndabs)
{
	for(int a = 0; a < m_size; a++){
		m_layers[a].initializeWeights(p_rndabs);
	}
}

void NeuralNet::updateWeights(const vector<const double *> &p_input, double p_learnrate)
{
	m_layers[0].updateWeights(p_input, p_learnrate);

	for(int a = 1; a < m_size; a++){
		m_layers[a].updateWeights(m_layers[a-1].getOutputRef(), p_learnrate);
	}
}



