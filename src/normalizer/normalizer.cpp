#include "normalizer.h"

Normalizer::Normalizer(int p_dim)
	:m_dim(p_dim), m_learning(true), m_mean(m_dim), m_deviation(m_dim), m_learnrate(0.3), m_learnratereduction(0.995)
{
	for(int a = 0; a < m_dim; a++){
		m_mean[a] = 0.0;
		m_deviation[a] = 1.0;
	}
}

vector<double> Normalizer::normalize(vector<double> p_input){

	//Check for dimension equality
	if(p_input.size() != m_dim){
		return vector<double>();
	}

	vector<double> output = vector<double>(m_dim);

	//Calculate output
	for(int a = 0; a < m_dim; a++){
		output[a] = (p_input[a] - m_mean[a]) / m_deviation[a];
	}

	//Adjust mean and deviation values
	if(m_learning){

		for(int a = 0; a < m_dim; a++){
			if(p_input[a] != m_mean[a]){
				//Learn values
				m_deviation[a] = (1.0 - m_learnrate / 2.0) * m_deviation[a] + m_learnrate * fabs(m_mean[a] - p_input[a]);
				m_mean[a] = (1.0 - m_learnrate) * m_mean[a] + m_learnrate * p_input[a];
			}
		}

		m_learnrate *= m_learnratereduction;
	}

	return output;
}

void Normalizer::setLearning(bool p_learning)
{
	m_learning = p_learning;
}
