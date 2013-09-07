#include "neuralnet.h"

NeuralNet::NeuralNet(int p_size, int p_layersize[], int p_inputsize)
	:m_size(p_size), m_layersize(p_layersize, p_layersize + p_size), m_inputsize(p_inputsize)
{
	m_layers.resize(m_size);
	m_input.resize(m_inputsize);
	m_input_ptr.resize(m_inputsize);

	for(int a = 0; a < m_inputsize; a++){
		m_input_ptr[a] = &(m_input[a]);
	}

	for(int a = 0; a < m_size; a++){
		if(a == 0)
			m_layers[a] = NeuronLayer(m_layersize[a], m_inputsize);
		else
			m_layers[a] = NeuronLayer(m_layersize[a], m_layersize[a-1]);
	}
}

double NeuralNet::calculate(double p_input[])
{
	m_input.assign(p_input, p_input + m_inputsize);

	calculateOutput(m_input_ptr);

	return *(m_layers[m_size-1].m_output_ref[0]);
}

void NeuralNet::calculateOutput(const vector<const double *> &p_input)
{
	m_layers[0].calculateOutput(p_input);

	for(int a = 1; a < m_size; a++){
		m_layers[a].calculateOutput(m_layers[a-1].m_output_ref);
	}
}



