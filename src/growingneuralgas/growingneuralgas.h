#ifndef GROWINGNEURALGAS_H
#define GROWINGNEURALGAS_H

#include <vector>
#include <list>
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
	Vertex(int p_dim)
		:position(p_dim), error(0.0), utility(0.0)
	{

	}

	Vertex(vector<double> p_position)
		:position(p_position)
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

	void learn(vector<double> p_input);
	double test(vector<double> p_input);

	int getDimension();
	const list<Vertex*> &getVertices();
	const list<Edge*> &getEdges();

private:
	const int m_dim;
	Random m_rand;

	int m_iterations;

	double m_attraction_fact_first;
	double m_attraction_fact_neighbors;
	int m_max_age;
	int m_generate_neuron;
	int m_max_vertices;
	double m_error_reduction;
	double m_error_reduction_new;

	list<Vertex*> m_vertices;
	list<Edge*> m_edges;
};

#endif // GROWINGNEURALGAS_H
