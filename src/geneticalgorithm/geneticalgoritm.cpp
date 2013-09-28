#include "geneticalgoritm.h"

GeneticAlgoritm::GeneticAlgoritm(int p_population_size, int p_chromosome_size, vector<double> p_min_val, vector<double> p_max_val, double (*p_fitness)(vector<double>))
	:m_population_size(p_population_size), m_chromosome_size(p_chromosome_size), m_evaluate_fitness(p_fitness)
{
	m_population.reserve(m_population_size);

	for(int a = 0; a < m_population_size; a++){
		m_population.push_back(Candidate(m_chromosome_size));
		m_population[a].initiate(p_min_val, p_max_val);
	}
}

vector<double> GeneticAlgoritm::runEvolution(double p_fitness_goal)
{
	int runs = 0;
	do{
		calculateFitness();
		int surviving = selectFittest();
		generatePopulation(surviving);
		runs++;
	}
	while(m_population[m_population_size-1].fitness < p_fitness_goal && runs < 200);

	return m_population[m_population_size-1].chromosomes;
}

void GeneticAlgoritm::calculateFitness()
{
	for(int a = 0; a < m_population_size; a++){
		m_population[a].fitness = m_evaluate_fitness(m_population[a].chromosomes);
	}
}

int GeneticAlgoritm::selectFittest()
{
	sort(m_population.begin(), m_population.end());
	return m_population_size / 2;
}

void GeneticAlgoritm::generatePopulation(int p_surviving)
{
	for(int a = 0; a < p_surviving; a++){
		m_population[a] = generateCandidate();
	}
}

Candidate GeneticAlgoritm::generateCandidate()
{
	Candidate candidate(m_chromosome_size);

	return candidate;
}
