#ifndef GENETICALGORITM_H
#define GENETICALGORITM_H

#include <vector>
#include <algorithm>
#include <stdlib.h>

using namespace std;

struct Candidate{
	Candidate(int p_chromosome_size){
		chromosomes.resize(p_chromosome_size);
		fitness = 0;
	}

	void initiate(vector<double> p_min_val, vector<double> p_max_val){
		for(int a = 0; a < chromosomes.size(); a++){
			chromosomes[a] = generateRandom(p_min_val[a], p_max_val[a]);
		}
	}

	static double generateRandom(double p_min, double p_max)
	{
		return (rand() % 1000000) / 1000000.0 * (p_max - p_min) + p_min;
	}

	bool operator < (const Candidate& rhs) const
	{
		return fitness < rhs.fitness;
	}

	bool operator > (const Candidate& rhs) const
	{
		return fitness > rhs.fitness;
	}

	vector<double> chromosomes;
	double fitness;
};

class GeneticAlgoritm
{
public:
	GeneticAlgoritm(int p_population_size, int p_chromosome_size, vector<double> p_min_val, vector<double> p_max_val, double (*p_fitness)(vector<double>));

	vector<double> runEvolution(double p_fitness_goal);

private:
	const int m_population_size;
	const int m_chromosome_size;

	double (*m_evaluate_fitness)(vector<double>);

	vector<Candidate> m_population;

	void calculateFitness();
	int selectFittest();
	void generatePopulation(int p_surviving);
	Candidate generateCandidate(int p_surviving);
	void mutatePopulation();
	void mutateCandidate(Candidate* p_candidate);

	static const double m_mutation_rate;
	static const double m_mutation_intensity;
	static const double m_crossover_rate;
	static const int m_parents;
};

#endif // GENETICALGORITM_H
