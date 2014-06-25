#include "normalizer.h"

#include <iostream>
using namespace std;

Normalizer::Normalizer(int p_dim)
	:m_dim(p_dim), m_learning(true), m_mean(m_dim), m_deviation(m_dim), m_learnrate(0.03), m_baselearnrate(0.001), m_learnratereduction(0.9995)
{
	for(int a = 0; a < m_dim; a++){
		m_mean[a] = 0.0;
		m_deviation[a] = 0.2;
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

		double learn = (m_baselearnrate + m_learnrate) / 2;
		double learn_deviation = learn / 2.0;

		for(int a = 0; a < m_dim; a++){
			if(p_input[a] - m_mean[a] > 2 * m_deviation[a]){
				//Learn values
				m_deviation[a] = (1.0 - learn_deviation) * m_deviation[a] + learn_deviation * (m_mean[a] - p_input[a]) * (m_mean[a] - p_input[a]);
				m_mean[a] = (1.0 - learn) * m_mean[a] + learn * p_input[a];
			}
		}

		m_learnrate *= m_learnratereduction;
	}

//	cout << "(";
//	for(int a = 0; a < m_dim; a++){
//		cout << output[a] << ", ";
//	}
//	cout << ")" << endl;


	return output;
}

void Normalizer::setLearning(bool p_learning)
{
	m_learning = p_learning;
}
