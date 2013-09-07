#include "neuron.h"

Neuron::Neuron()
	:m_size(0), m_function(Function_Tanh), m_output_ref(&m_output)
{

}

Neuron::Neuron(int p_size)
	:m_size(p_size), m_function(Function_Tanh), m_output_ref(&m_output)
{
	m_weights.resize(m_size);
	//Initiallize weights
}

void Neuron::calculateOutput(const vector<const double *> &p_input)
{
	m_output = 0.0;
	m_output += m_bias;

	for(int a = 0; a < m_size; a++){
		m_output += *(p_input[a]) * m_weights[a];
	}

	m_output = m_function.m_function(m_output);
}
