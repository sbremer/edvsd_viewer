#include "normalizer.h"

Normalizer::Normalizer(int p_dim)
	:m_dim(p_dim), m_learning(true), m_mean(m_dim), m_deviation(m_dim), m_learnrate(0.3), m_baselearnrate(0.01), m_learnratereduction(0.998)
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

		double learn = m_baselearnrate + m_learnrate;

		for(int a = 0; a < m_dim; a++){
			if(p_input[a] != m_mean[a]){
				//Learn values
				m_deviation[a] = (1.0 - learn) * m_deviation[a] + learn * fabs(m_mean[a] - p_input[a]);
				m_mean[a] = (1.0 - learn) * m_mean[a] + learn * p_input[a];
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
