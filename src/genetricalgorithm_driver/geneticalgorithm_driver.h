#ifndef GENETICALGORITHM_DRIVER_H
#define GENETICALGORITHM_DRIVER_H

#include <vector>

#include "helper/pointf.h"
#include "helper/eventf.h"
#include "geneticalgorithm/geneticalgorithm.h"
#include "edvsd_analyzer.h"

using namespace std;

class GeneticAlgorithm_Driver
{
public:
	GeneticAlgorithm_Driver(EventF *p_buffer, int p_n, double p_deviation);
	~GeneticAlgorithm_Driver();
	void runGeneticAlgorithm();

	double evaluatePerformance(vector<double> p_settings);

private:
	const int m_size;
	const int m_runs;

	EventF *m_buffer;
	int m_n;

	vector<double> m_setup;
	vector<double> m_min;
	vector<double> m_max;

	GeneticAlgorithm *m_genetic_algorithm;
};

#endif // GENETICALGORITHM_DRIVER_H
