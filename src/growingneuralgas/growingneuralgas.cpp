#include "growingneuralgas.h"

GrowingNeuralGas::GrowingNeuralGas(int p_dim)
	:m_dim(p_dim), m_attraction_fact_first(0.2), m_attraction_fact_neighbors(0.01), m_max_age(20), m_generate_neuron(100), m_max_vertices(200), m_error_reduction(0.99), m_error_reduction_new(0.5), m_error_reduction_dim(0.01), m_rand()
	//:m_dim(p_dim), m_attraction_fact_first(0.2), m_attraction_fact_neighbors(0.006), m_max_age(50), m_generate_neuron(100), m_error_reduction(0.995), m_error_reduction_new(0.5), m_rand()
{
	m_iterations = 1;

	//Initiate
	vector<double> init(m_dim);

	for(int a = 0; a < m_dim; a++){
		init[a] = m_rand.randomDouble(-1.0, 1.0);
	}

	Vertex *vert1 = new Vertex(init);
	m_vertices.push_back(vert1);

	for(int a = 0; a < m_dim; a++){
		init[a] = m_rand.randomDouble(-1.0, 1.0);
	}

	Vertex *vert2 = new Vertex(init);
	m_vertices.push_back(vert2);

	Edge *edge = new Edge(vert1, vert2);

	vert1->edges.push_back(edge);
	vert2->edges.push_back(edge);
	m_edges.push_back(edge);
}

void GrowingNeuralGas::learn(vector<double> p_input)
{
//	if(m_iterations == 200){
//		m_attraction_fact_first = 0.2;
//		m_attraction_fact_neighbors = 0.008;
//	}
//	m_attraction_fact_first *= 0.999;
//	m_attraction_fact_neighbors *= 0.999;

	//Find first (s1) and second (s1) closest vertex
	Vertex *s1 = 0;
	Vertex *s2 = 0;
	double distmin = INFINITY;
	double distmin2 = INFINITY;

	for(list<Vertex*>::iterator iter = m_vertices.begin(); iter != m_vertices.end(); iter++){

		//Reduce error of all vertices
		(*iter)->error *= m_error_reduction;
		(*iter)->utility *= m_error_reduction;

		double dist = (*iter)->getDistance(p_input);
		if(dist < distmin){
			s2 = s1;
			s1 = *iter;
			distmin2 = distmin;
			distmin = dist;
		}
		else if(dist < distmin2){
			s2 = *iter;
			distmin2 = dist;
		}
	}

	//Calculate error for s1
	s1->error += distmin * distmin;
	s1->utility += distmin2 * distmin2 - distmin * distmin;

	//Adjusting position of s1
	for(int a = 0; a < m_dim; a++){
		s1->position[a] += m_attraction_fact_first * (p_input[a] - s1->position[a]);

		s1->error_dim[a] = fabs(s1->position[a] - p_input[a]) * m_error_reduction_dim + (1.0 - m_error_reduction_dim) * s1->error_dim[a];
	}

	//Increase age of edges of s1 + Adjust position of neighbor vertices
	//Check for edge between s1 and s2, create if nonexistant, set age to zero if existant
	Edge *edge = 0;
	for(list<Edge*>::iterator iter = s1->edges.begin(); iter != s1->edges.end(); iter++){
		(*iter)->age++;

		for(int a = 0; a < m_dim; a++){
			if((*iter)->vertex1 != s1){
				(*iter)->vertex1->position[a] += m_attraction_fact_neighbors * (p_input[a] - (*iter)->vertex1->position[a]);
				if((*iter)->vertex1->utility == 0){
					(*iter)->vertex1->utility = 0.001;
				}
			}
			else{
				(*iter)->vertex2->position[a] += m_attraction_fact_neighbors * (p_input[a] - (*iter)->vertex2->position[a]);
				if((*iter)->vertex2->utility == 0){
					(*iter)->vertex2->utility = 0.001;
				}
			}
		}
	}

	for(list<Edge*>::iterator iter = s1->edges.begin(); iter != s1->edges.end(); iter++){
		if((*iter)->vertex1 == s2 || (*iter)->vertex2 == s2){
			edge = (*iter);
			break;
		}
	}

	if(edge != 0){
		//Set age to zero
		edge->age = 0;
	}
	else{
		//Create edge between s1 and s2
		edge = new Edge(s1, s2);
		m_edges.push_back(edge);
		s1->edges.push_back(edge);
		s2->edges.push_back(edge);
	}

	//Remove edges older than ... (Maybe not every iteration?)
	for(list<Edge*>::iterator iter = m_edges.begin(); iter != m_edges.end(); /*iter++*/){
		if(m_iterations % 50 == 0){
//			(*iter)->age++;
		}

		if((*iter)->age > m_max_age){
			(*iter)->vertex1->edges.remove(*iter);
			(*iter)->vertex2->edges.remove(*iter);
			//m_edges.remove(*iter);

			//Remove points if no edges left
			if((*iter)->vertex1->edges.size() == 0){
				m_vertices.remove((*iter)->vertex1);
				delete (*iter)->vertex1;
			}
			if((*iter)->vertex2->edges.size() == 0){
				m_vertices.remove((*iter)->vertex2);
				delete (*iter)->vertex2;
			}

			delete *iter;
			iter = m_edges.erase(iter);
		}
		else{
			iter++;
		}
	}

//	//Remove vertices
//	if(m_iterations % (m_generate_neuron*20) == 0 && m_iterations > 300){
//		for(list<Vertex*>::iterator iter = m_vertices.begin(); iter != m_vertices.end(); /*iter++*/){
//			if((*iter)->utility == 0.0){

//				for(list<Edge*>::iterator iter2 = (*iter)->edges.begin(); iter2 != (*iter)->edges.end(); iter2++){
//					if((*iter2)->vertex1 != (*iter)){
//						(*iter2)->vertex1->edges.remove((*iter2));
//						if((*iter2)->vertex1->edges.size() == 0){
//							m_vertices.remove((*iter2)->vertex1);
//							delete (*iter2)->vertex1;
//						}
//					}
//					else{
//						(*iter2)->vertex2->edges.remove((*iter2));
//						if((*iter2)->vertex2->edges.size() == 0){
//							m_vertices.remove((*iter2)->vertex2);
//							delete (*iter2)->vertex2;
//						}
//					}

//					m_edges.remove((*iter2));
//					delete (*iter2);
//				}

//				delete *iter;
//				iter = m_vertices.erase(iter);
//				//break;
//			}
//			else{
//				iter++;
//			}
//		}
//	}

	//Add vertex
	if(m_iterations % m_generate_neuron == 0 && m_vertices.size() < m_max_vertices){
		Vertex *q = 0;
		double errormax = 0;

		for(list<Vertex*>::iterator iter = m_vertices.begin(); iter != m_vertices.end(); iter++){
			if((*iter)->error > errormax){
				q = *iter;
				errormax = (*iter)->error;
			}

			//(*iter)->utility = 0.0;
		}

		Vertex *f = 0;
		Edge *c = 0;
		errormax = 0;

		for(list<Edge*>::iterator iter = q->edges.begin(); iter != q->edges.end(); iter++){
			if((*iter)->vertex1 != q){
				if((*iter)->vertex1->error >= errormax){
					f = (*iter)->vertex1;
					c = (*iter);
					errormax = (*iter)->vertex1->error;
				}
			}
			else{
				if((*iter)->vertex2->error >= errormax){
					f = (*iter)->vertex2;
					c = (*iter);
					errormax = (*iter)->vertex2->error;
				}
			}
		}

		q->edges.remove(c);
		f->edges.remove(c);
		m_edges.remove(c);
		delete c;

		vector<double> new_pos(m_dim);

		for(int a = 0; a < m_dim; a++){
			new_pos[a] = (q->position[a] + f->position[a]) / 2.0;
		}

		Vertex *n = new Vertex(new_pos);
		m_vertices.push_back(n);

		//n->error = (q->error + f->error) / 2.0;
		q->error *= m_error_reduction_new;
		f->error *= m_error_reduction_new;
		n->error = q->error;

		Edge *qn = new Edge(q, n);
		m_edges.push_back(qn);
		q->edges.push_back(qn);
		n->edges.push_back(qn);

		Edge *nf = new Edge(n, f);
		m_edges.push_back(nf);
		n->edges.push_back(nf);
		f->edges.push_back(nf);
	}
	m_iterations++;
}

double GrowingNeuralGas::test(vector<double> p_input)
{
	//Find first (s1) and second (s1) closest vertex
	Vertex *s1 = 0;
	Vertex *s2 = 0;
	double distmin = INFINITY;
	double distmin2 = INFINITY;

	for(list<Vertex*>::iterator iter = m_vertices.begin(); iter != m_vertices.end(); iter++){
		double dist = (*iter)->getDistance(p_input);
		if(dist < distmin){
			s2 = s1;
			s1 = *iter;
			distmin2 = distmin;
			distmin = dist;
		}
		else if(dist < distmin2){
			s2 = *iter;
			distmin2 = dist;
		}
	}

	double error = 0.0;

	for(int a = 0; a < m_dim; a++){
		if(s1->error_dim[a] != 0.0){
			error += fabs(s1->position[a] - p_input[a]);// / s1->error_dim[a];
			//error += fabs(s2->position[a] - p_input[a]) / 2.0;
		}
	}

	error /= m_dim * 2;

	return error;
}

int GrowingNeuralGas::getDimension()
{
	return m_dim;
}

const list<Vertex*> &GrowingNeuralGas::getVertices()
{
	return m_vertices;
}

const list<Edge*> &GrowingNeuralGas::getEdges()
{
	return m_edges;
}
