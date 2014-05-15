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

struct VertexLink{
	VertexLink(Vertex *p_link)
		:link(p_link), atime_transfer(0.0), atime_transfer_deviation(1000.0), last_transfer(0), atime_stay(0.0), atime_stay_deviation(1000.0)
	{

	}

	const Vertex *link;

	double atime_transfer;
	double atime_transfer_deviation;
	unsigned int last_transfer;

	double atime_stay;
	double atime_stay_deviation;

	int age;
};

struct Vertex{
	Vertex(vector<double> p_position)
		:position(p_position), error(0.0), utility(0.0), error_dim(p_position.size()), atime_newnode(0.0), atime_newnode_deviation(1000.0), last_new(0), atime_stay_kill(0.0), atime_stay_kill_deviation(1000.0), atime_killnode(0.0), atime_killnode_deviation(1000.0), last_kill(0)
	{
		for(int a = 0; a < position.size(); a++){
			error_dim[a] = 1.0;
		}
		links = list<VertexLink*>();
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

	double atime_stay_kill;
	double atime_stay_kill_deviation;

	double atime_killnode;
	double atime_killnode_deviation;
	unsigned int last_kill;

	list<VertexLink*> links;
};

struct InputNode{
	InputNode(int p_id, unsigned int p_time)
		:id(p_id), since(p_time)
	{

	}

	const int id;
	Vertex *current;
	unsigned int since;
};

class GrowingNeuralGas
{
public:
	GrowingNeuralGas(int p_dim);
	GrowingNeuralGas(int p_dim, double p_attraction_fact_first, double p_attraction_fact_neighbors, int p_max_age, int p_generate_neuron, int p_max_vertices, int p_age_edges);

	void newNode(vector<double> p_input, int p_id, unsigned int p_time);
	void learnNode(vector<double> p_input, int p_id, unsigned int p_time);
	void killNode(int p_id, unsigned int p_time);

	double test_newNode(vector<double> p_input, int p_id, unsigned int p_time);
	double test_learnNode(vector<double> p_input, int p_id, unsigned int p_time);
	double test_killNode(int p_id, unsigned int p_time);

	//replacement
	void learn(vector<double> p_input);
	double test(vector<double> p_input);

	int getDimension();
	const list<Vertex*> &getVertices();
	const list<Edge*> &getEdges();

private:
	Vertex* adjustGNG(vector<double> p_input);
	double accumulateError(double p_total, double p_component);

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
	list<InputNode*> m_inputnodes;
};

#endif // GROWINGNEURALGAS_H
