#include "neuronlayer.h"

NeuronLayer::NeuronLayer()
	:m_size(0), m_output_ref(m_output)
{

}

NeuronLayer::NeuronLayer(int p_size, int p_size_beneath)
	:m_size(p_size), m_output_ref(m_output)
{
	m_neurons.resize(m_size);
	m_output.resize(m_size);

	for(int a = 0; a < m_size; a++){
		m_neurons[a] = Neuron(p_size_beneath);
		m_output[a] = m_neurons[a].m_output_ref;
	}
}

void NeuronLayer::calculateOutput(const vector<const double *> &p_input)
{
	for(int a = 0; a < m_size; a++){
		m_neurons[a].calculateOutput(p_input);
	}
}
