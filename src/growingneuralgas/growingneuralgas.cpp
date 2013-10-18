#include "growingneuralgas.h"

GrowingNeuralGas::GrowingNeuralGas(int p_dim)
	:m_dim(p_dim)
{

}

void GrowingNeuralGas::learn(vector<double> p_input)
{
	//Find first (s1) and second (s1) closest vertex
	Vertex *s1 = 0;
	Vertex *s2 = 0;
	double distmin = INFINITY;

	for(list<Vertex>::iterator iter = m_vertices.begin(); iter != m_vertices.end(); iter++){
		if(iter->getDistance(p_input) < distmin){
			s2 = s1;
			s1 = &(*iter);
		}
	}

	//Calculate error for s1
	s1->error = distmin * distmin;

	//Adjusting position of s1
	for(int a = 0; a < m_dim; a++){
		s1->position[a] += 0.1 * (p_input[a] - s1->position[a]);
	}

	//Increase age of edges of s1 + Adjust position of neighbor vertices
	for(list<Edge*>::iterator iter = s1->edges.begin(); iter != s1->edges.end(); iter++){
		(*iter)->age++;

		for(int a = 0; a < m_dim; a++){
			if((*iter)->vertex1 != s1){
				(*iter)->vertex1->position[a] += 0.01 * (p_input[a] - (*iter)->vertex1->position[a]);
			}
			else{
				(*iter)->vertex2->position[a] += 0.01 * (p_input[a] - (*iter)->vertex2->position[a]);
			}
		}
	}

	//Check for edge between s1 and s2, create if nonexistant, set age to zero if existant
	Edge *edge = 0;
	for(list<Edge*>::iterator iter = s1->edges.begin(); iter != s1->edges.end(); iter++){
		if((*iter)->vertex1 == s2 || (*iter)->vertex2){
			edge = (*iter);
			break;
		}
	}

	if(edge != 0){
		edge->age = 0;
	}
	else{
		edge = new Edge(s1, s2);
		s1->edges.push_back(edge);
		s2->edges.push_back(edge);
	}

	//Remove edges older than ...

}
