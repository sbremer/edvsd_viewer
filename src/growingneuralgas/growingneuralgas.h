#ifndef GROWINGNEURALGAS_H
#define GROWINGNEURALGAS_H

#include <vector>
#include <list>
#include <algorithm>
#include <math.h>

#include "helper/random.h"

using namespace std;

struct Vertex;

struct Edge{
	Edge(Vertex *p_vertex1, Vertex *p_vertex2)
	{
		vertex1 = p_vertex1;
		vertex2 = p_vertex2;
		age = 0;
	}

	Vertex *vertex1;
	Vertex *vertex2;
	int age;
};

struct Vertex{
	Vertex(vector<double> p_position)
		:position(p_position), error(0.0), utility(0.0)
	{

	}

	double getDistance(vector<double> p_input)
	{
		double dist = 0;

		for(int a = 0; a < position.size(); a++){
			dist += (p_input[a] - position[a]) * (p_input[a] - position[a]);
		}

		dist = sqrt(dist);

		return dist;
	}

	vector<double> position;
	list<Edge*> edges;
	double error;
	double utility;
};

class GrowingNeuralGas
{
public:
	GrowingNeuralGas(int p_dim);
	GrowingNeuralGas(int p_dim, double p_attraction_fact_first, double p_attraction_fact_neighbors, int p_max_age, int p_generate_neuron, int p_max_vertices, int p_age_edges);

	void learn(vector<double> p_input);
	double test(vector<double> p_input);

	int getDimension();
	const list<Vertex*> &getVertices();
	const list<Edge*> &getEdges();

private:
	const int m_dim;
	Random m_rand;

	int m_iterations;

	const double m_attraction_fact_first;
	const double m_attraction_fact_neighbors;
	const int m_max_age;
	const int m_generate_neuron;
	const int m_max_vertices;
	const int m_age_edges;
	const double m_error_reduction;
	const double m_error_reduction_new;

	double m_error_reduction_dim;

	list<Vertex*> m_vertices;
	list<Edge*> m_edges;
};

#endif // GROWINGNEURALGAS_H
