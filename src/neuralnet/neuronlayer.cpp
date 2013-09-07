#include "neuronlayer.h"

NeuronLayer::NeuronLayer()
	:m_size(0), m_output_ref(m_output), m_delta_ref(m_delta)
{

}

NeuronLayer::NeuronLayer(int p_size, int p_size_beneath)
	:m_size(p_size), m_output_ref(m_output), m_delta_ref(m_delta)
{
	m_neurons.reserve(m_size);
	m_output.resize(m_size);
	m_delta.resize(m_size);

	for(int a = 0; a < m_size; a++){
		m_neurons.push_back(Neuron(p_size_beneath));
		m_output[a] = m_neurons[a].m_output_ref;
		m_delta[a] = &(m_neurons[a].m_delta);
	}
}

void NeuronLayer::calculateOutput(const vector<const double *> &p_input)
{
	for(int a = 0; a < m_size; a++){
		m_neurons[a].calculateOutput(p_input);
	}
}

void NeuronLayer::calculateError(const vector<double const*> &p_exp_output)
{
	for(int a = 0; a < m_size; a++){
		*(m_delta[a]) = *(m_output[a]) - *(p_exp_output[a]);
	}
}

void NeuronLayer::calculateDelta(const vector<double *> &p_delta_ref)
{
	for(int a = 0; a < p_delta_ref.size(); a++){
		*(p_delta_ref[a]) = 0.0;
	}

	for(int a = 0; a < m_size; a++){
		m_neurons[a].calculateDelta(p_delta_ref);
	}
}

void NeuronLayer::updateWeights(const vector<const double *> &p_input, double p_learnrate)
{
	for(int a = 0; a < m_size; a++){
		m_neurons[a].updateWeights(p_input, p_learnrate);
	}
}
