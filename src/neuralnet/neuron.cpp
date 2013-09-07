#include "neuron.h"

Neuron::Neuron()
	:m_size(0), m_activation_function(Function_Tanh), m_output_ref(&m_output)
{

}

Neuron::Neuron(int p_size)
	:m_size(p_size), m_activation_function(Function_Tanh), m_output_ref(&m_output)
{
	m_weights.resize(m_size);
	//Initiallize weights
}

void Neuron::calculateOutput(const vector<const double *> &p_input)
{
	m_net = 0.0;
	m_net += m_bias;

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

void Neuron::updateWeights(const vector<const double *> &p_input, double p_learnrate)
{
	for(int a = 0; a < m_size; a++){
		m_weights[a] += p_learnrate * m_delta * m_activation_function.m_function_derivative(m_net) * *(p_input[a]);
	}

	m_bias += p_learnrate * m_delta * m_activation_function.m_function_derivative(m_net);
}
