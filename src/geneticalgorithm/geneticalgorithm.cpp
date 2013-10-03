#include "geneticalgorithm.h"

#include <iostream>
using namespace std;

GeneticAlgorithm::GeneticAlgorithm()
	:m_population_size(0), m_chromosome_size(0)
{

}

GeneticAlgorithm::GeneticAlgorithm(const int p_population_size, const int p_chromosome_size, vector<double> p_min_val, vector<double> p_max_val, double (*p_fitness)(vector<double>))
	:m_population_size(p_population_size), m_chromosome_size(p_chromosome_size), m_evaluate_fitness(p_fitness)
{
	m_population.reserve(m_population_size);

	for(int a = 0; a < m_population_size; a++){
		m_population.push_back(Candidate(m_chromosome_size));
		m_population[a].initiate(p_min_val, p_max_val);
	}
}

const double GeneticAlgorithm::m_mutation_rate = 0.1;
const double GeneticAlgorithm::m_mutation_intensity = 0.05;
const double GeneticAlgorithm::m_crossover_rate = 0.3;
const int GeneticAlgorithm::m_parents = 2;

vector<double> GeneticAlgorithm::runEvolution(double p_fitness_goal, int p_max_runs)
{
	int runs = 0;
	do{
		calculateFitness();
		int surviving = selectFittest();
		generatePopulation(surviving);
		mutatePopulation();
		runs++;

		for(int a = 0; a < m_population_size; a++){
			cout << "Run: " << runs << " Cand:" << a << "   Performance: " << m_population[a].fitness << endl;
		}
		//cout << "Run: " << runs << "   Performance: " << m_population[m_population_size-1].fitness << endl;
		flush(cout);

	}
	while(m_population[m_population_size-1].fitness < p_fitness_goal && runs < p_max_runs);

	return m_population[m_population_size-1].chromosomes;
}

void GeneticAlgorithm::calculateFitness()
{
	for(int a = 0; a < m_population_size; a++){
		m_population[a].fitness = m_evaluate_fitness(m_population[a].chromosomes);
	}
}

int GeneticAlgorithm::selectFittest()
{
	sort(m_population.begin(), m_population.end());
	return m_population_size / 2;
}

void GeneticAlgorithm::generatePopulation(int p_surviving)
{
	for(int a = 0; a < p_surviving; a++){
		m_population[a] = generateCandidate(p_surviving);
	}
}

Candidate GeneticAlgorithm::generateCandidate(int p_surviving)
{
	Candidate candidate(m_chromosome_size);
	Candidate *parent[m_parents];

	int parentrange = m_population_size - p_surviving;
	for(int a = 0; a < m_parents; a++){
		//Todo: Logarithmic
		parent[a] = &(m_population[rand() % parentrange]);
	}

	for(int a = 0; a < m_chromosome_size; a++){
		if((rand() % 10000) / 10000.0 <= m_crossover_rate){
			candidate.chromosomes[a] =	parent[rand() % (m_parents - 1) + 1]->chromosomes[a];
		}
		else{
			candidate.chromosomes[a] =	parent[0]->chromosomes[a];
		}
	}

	return candidate;
}

void GeneticAlgorithm::mutatePopulation()
{
	for(int a = 0; a < m_population_size; a++){
		mutateCandidate(&(m_population[a]));
	}
}

void GeneticAlgorithm::mutateCandidate(Candidate *p_candidate)
{
	//Todo: mutate depending on position in sorted vector
	for(int a = 0; a < m_chromosome_size; a++){
		if((rand() % 10000) / 10000.0 <= m_mutation_rate){
			double mutation = p_candidate->chromosomes[a] * m_mutation_intensity;
			if(mutation < 0.0)mutation *= -1;
			p_candidate->chromosomes[a] += Candidate::generateRandom(-1 * mutation, mutation);
		}
	}
}
