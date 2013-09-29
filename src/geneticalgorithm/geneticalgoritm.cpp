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

const double GeneticAlgoritm::m_mutation_rate = 0.1;
const double GeneticAlgoritm::m_mutation_intensity = 0.05;
const double GeneticAlgoritm::m_crossover_rate = 0.3;
const int GeneticAlgoritm::m_parents = 2;

vector<double> GeneticAlgoritm::runEvolution(double p_fitness_goal)
{
	int runs = 0;
	do{
		calculateFitness();
		int surviving = selectFittest();
		generatePopulation(surviving);
		mutatePopulation();
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
		m_population[a] = generateCandidate(p_surviving);
	}
}

Candidate GeneticAlgoritm::generateCandidate(int p_surviving)
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

void GeneticAlgoritm::mutatePopulation()
{
	for(int a = 0; a < m_population_size; a++){
		mutateCandidate(&(m_population[a]));
	}
}

void GeneticAlgoritm::mutateCandidate(Candidate *p_candidate)
{
	for(int a = 0; a < m_chromosome_size; a++){
		if((rand() % 10000) / 10000.0 <= m_mutation_rate){
			double mutation = p_candidate->chromosomes[a] + m_mutation_intensity;
			if(mutation < 0.0)mutation *= -1;
			p_candidate->chromosomes[a] += Candidate::generateRandom(-1 * mutation, mutation);
		}
	}
}
