#include "neuron.h"

//For Debugging
#include "iostream"
using namespace std;

Neuron::Neuron()
	:m_size(0), m_activation_function(Function_Tanh)
{

}

Neuron::Neuron(int p_size)
	:m_size(p_size), m_activation_function(Function_Tanh)
{
	m_weights.resize(m_size);
}

Neuron::Neuron(int p_size, ActivationFunction p_actication_function)
	:m_size(p_size), m_activation_function(p_actication_function)
{
	m_weights.resize(m_size);
}

double const * const Neuron::getOutputRef()
{
	return &m_output;
}

void Neuron::calculateOutput(const vector<const double *> &p_input)
{
	m_net = 0.0;
	m_net += m_bias;

	//cout << *(p_input.at(0)) << endl;

	for(int a = 0; a < m_size; a++){
		m_net += *(p_input[a]) * m_weights[a];
	}

	m_output = m_activation_function.m_function(m_net);
}

void Neuron::calculateDelta(const vector<double *> &p_delta_ref)
{
	for(int a = 0; a < p_delta_ref.size(); a++){
		*(p_delta_ref[a]) += m_delta * m_weights[a];
	}
}

void Neuron::initializeWeights(double p_rndabs)
{
	for(int a = 0; a < m_size; a++){
		m_weights[a] = generateRandom(p_rndabs);
	}

	m_bias = generateRandom(2.0);
}

void Neuron::updateWeights(const vector<const double *> &p_input, double p_learnrate)
{
	for(int a = 0; a < m_size; a++){
		m_weights[a] += p_learnrate * m_delta * m_activation_function.m_function_derivative(m_net) * *(p_input[a]);
	}

	m_bias += p_learnrate * m_delta * m_activation_function.m_function_derivative(m_net);
}

double Neuron::generateRandom(double p_rndabs)
{
	return (rand()%1000000) / 500000.0 * p_rndabs - p_rndabs;
}
