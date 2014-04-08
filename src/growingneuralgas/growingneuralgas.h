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

struct VertexLink{
	Vertex *link;
	double probability; //Or rather use rate? Prob can still be calculated
	double atime_stay;
	double atime_stay_deviation;
	int age;
};

struct Vertex{
	Vertex(int p_dim)
		:position(p_dim), error(0.0), utility(0.0), error_dim(p_dim)
	{
		for(int a = 0; a < p_dim; a++){
			position[a] = 0.0;
			error_dim[a] = 1.0;
		}
	}

	Vertex(vector<double> p_position)
		:position(p_position), error(0.0), utility(0.0), error_dim(p_position.size())
	{
		for(int a = 0; a < position.size(); a++){
			error_dim[a] = 1.0;
		}
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
	vector<double> error_dim;
	double utility;

	double atime_newnode;
	double atime_newnode_deviation;
	unsigned int last_new;

	double atime_killnode;
	double atime_killnode_deviation;
	unsigned int last_kill;

	list<VertexLink> links;
};

struct InputNode{
	InputNode(int p_id, unsigned int p_time)
		:id(p_id), since(p_time)
	{

	}

	int id;
	Vertex *current;
	unsigned int since;
};

class GrowingNeuralGas
{
public:
	GrowingNeuralGas(int p_dim);

	void newNode(vector<double> p_input, int p_id, unsigned int p_time);
	void learnNode(vector<double> p_input, int p_id, unsigned int p_time);
	void killNode(int p_id, unsigned int p_time);

	double testNode(vector<double> p_input, int p_id, unsigned int p_time);

	int getDimension();
	const list<Vertex*> &getVertices();
	const list<Edge*> &getEdges();

private:
	Vertex* adjustGNG(vector<double> p_input);

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

	double m_error_reduction_dim;

	list<Vertex*> m_vertices;
	list<Edge*> m_edges;
	list<InputNode*> m_inputnodes;
};

#endif // GROWINGNEURALGAS_H
