#include "geneticalgorithm_driver.h"

#include <iostream>
using namespace std;

GeneticAlgorithm_Driver *driver = NULL;

double evaluatePerformance_Wrapper(vector<double> p_settings)
{
	return driver->evaluatePerformance(p_settings);
}

GeneticAlgorithm_Driver::GeneticAlgorithm_Driver(EventF *p_buffer, int p_n, double p_deviation)
	:m_size(16), m_runs(1), m_buffer(p_buffer), m_n(p_n)
{
	m_setup.resize(m_size);
	m_min.resize(m_size);
	m_max.resize(m_size);

	//Parameter Setup
	m_setup[0] = 2.5; //StartEndTracker::m_attraction_fact_start
	m_setup[1] = 3.2; //StartEndTracker::m_attraction_pow_start
	m_setup[2] = 1.0; //StartEndTracker::m_dist
	m_setup[3] = 2.0; //StartEndTracker::m_attraction_fact_end
	m_setup[4] = 3.3; //StartEndTracker::m_attraction_pow_end

	m_setup[5] = 4.0; //KohonenTracking::m_start_dist
	m_setup[6] = 4.0; //KohonenTracking::m_end_dist
	m_setup[7] = 3.0; //KohonenTracking::m_attraction_fact
	m_setup[8] = 3.0; //KohonenTracking::m_attraction_pow
	m_setup[9] = 0.3; //KohonenTracking::m_attraction_max
	m_setup[10] = 0.01; //KohonenTracking::m_neighbor_attraction

	m_setup[11] = 0.3; //NeuralNet_Driver::m_learnrate
	m_setup[12] = 0.001; //NeuralNet_Driver::m_learnrate_reduction (*)
	m_setup[13] = 0.07; //NeuralNet_Driver::rnd
	m_setup[14] = 2.0; //NeuralNet_Driver::rnd_bias
	m_setup[15] = 10; //NeuralNet_Driver::neurons  (*)

	for(int a = 0; a < m_size; a++){
		m_min[a] = m_setup[a] * (1.0 - p_deviation);
		m_max[a] = m_setup[a] * (1.0 + p_deviation);
	}

	m_genetic_algorithm = new GeneticAlgorithm(10, m_size, m_min, m_max, &evaluatePerformance_Wrapper);
}

GeneticAlgorithm_Driver::~GeneticAlgorithm_Driver()
{
	delete m_genetic_algorithm;
}

void GeneticAlgorithm_Driver::runGeneticAlgorithm()
{
	driver = this;
	vector<double> best = m_genetic_algorithm->runEvolution(10000.0, 30);
	for(int a = 0; a < m_size; a++){
		cout << "best[" << a << "] = " << best[a] << ";" << endl;
	}
}

double GeneticAlgorithm_Driver::evaluatePerformance(vector<double> p_settings)
{
	int runs = 5;
	double performance = 0;
	for(int a = 0; a < runs; a++){
		EDVSD_Analyzer analyzer(p_settings);
		performance += analyzer.analyze(m_buffer, m_n);
	}
	return performance / runs;
}

