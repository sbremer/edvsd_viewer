#include "growingneuralgas.h"

#include <iostream>
using namespace std;

GrowingNeuralGas::GrowingNeuralGas(int p_dim)
	:m_dim(p_dim), m_attraction_fact_first(0.15), m_attraction_fact_neighbors(0.01), m_max_age(50), m_generate_neuron(50), m_max_vertices(200), m_age_edges(0), m_error_reduction(0.99), m_error_reduction_new(0.5), m_error_reduction_dim(0.01), m_rand()
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

GrowingNeuralGas::GrowingNeuralGas(int p_dim, double p_attraction_fact_first, double p_attraction_fact_neighbors, int p_max_age, int p_generate_neuron, int p_max_vertices, int p_age_edges)
	:m_dim(p_dim), m_attraction_fact_first(p_attraction_fact_first), m_attraction_fact_neighbors(p_attraction_fact_neighbors), m_max_age(p_max_age), m_generate_neuron(p_generate_neuron), m_max_vertices(p_max_vertices), m_age_edges(p_age_edges), m_error_reduction(0.99), m_error_reduction_new(0.5), m_error_reduction_dim(0.01), m_rand()
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
	adjustGNG(p_input);
}

double GrowingNeuralGas::test(vector<double> p_input)
{
	//Find first (s1) and second (s1) closest vertex
	Vertex *s1 = 0;
	Vertex *s2 = 0;
	Vertex *s3 = 0;
	double distmin = INFINITY;
	double distmin2 = INFINITY;
	double distmin3 = INFINITY;

	for(list<Vertex*>::iterator iter = m_vertices.begin(); iter != m_vertices.end(); iter++){
		double dist = (*iter)->getDistance(p_input);
		if(dist < distmin){
			s3 = s2;
			s2 = s1;
			s1 = *iter;
			distmin3 = distmin2;
			distmin2 = distmin;
			distmin = dist;
		}
		else if(dist < distmin2){
			s3 = s2;
			s2 = *iter;
			distmin3 = distmin2;
			distmin2 = dist;
		}
		else if(dist < distmin3){
			s3 = *iter;
			distmin3 = dist;
		}
	}

	if(s3 == NULL){
		return 0.0;
	}

	double rel_distance = 0.0;

	rel_distance = (s1->getDistance(s2->position) + s1->getDistance(s3->position) + s3->getDistance(s2->position)) / 3.0;

	return distmin / rel_distance;

	//Calculate average deviations of all dimensions
	vector<double> deviation = vector<double>(m_dim);
	for(int a = 0; a < m_dim; a++){
		deviation[a] = (s1->position[a] - s2->position[a]) * (s1->position[a] - s2->position[a]);
		deviation[a] = max(deviation[a], (s1->position[a] - s3->position[a]) * (s1->position[a] - s3->position[a]));
		deviation[a] = max(deviation[a], (s2->position[a] - s3->position[a]) * (s2->position[a] - s3->position[a]));
	}

	//Calculate relative error
	double error = 0.0;
	double partial = 0.0;

	double maxpartial = 0.0;
	int maxdim = -1;

	for(int a = 0; a < m_dim; a++){
		partial = (p_input[a] - s1->position[a]) * (p_input[a] - s1->position[a]) / deviation[a];
		partial = min(partial, (p_input[a] - s2->position[a]) * (p_input[a] - s2->position[a]) / deviation[a]);
		partial = min(partial, (p_input[a] - s3->position[a]) * (p_input[a] - s3->position[a]) / deviation[a]);

		if(partial > maxpartial){
			maxpartial = partial;
			maxdim = a;
		}

		error += partial;
	}

	error = sqrt(error / m_dim);

	if(error > 20.0){
		cout << "Error(" << maxdim << ") " << error << endl;
		cout << distmin / s1->getDistance(s2->position) << endl;
	}

	return error;
}

void GrowingNeuralGas::newNode(vector<double> p_input, int p_id, unsigned int p_time)
{
	InputNode *node = new InputNode(p_id, p_time);
	m_inputnodes.push_back(node);

	Vertex *closest = adjustGNG(p_input);
	node->current = closest;

	if(closest->last_new == 0){
		closest->last_new = p_time;
	}
	else{
		//Adjust closest new rate
		double dt = p_time - closest->last_new;
		closest->last_new = p_time;

		double atime_imp = 0.1;

		closest->atime_newnode_deviation = (1.0 - atime_imp) * closest->atime_newnode_deviation + atime_imp * (closest->atime_newnode - dt) * (closest->atime_newnode - dt);
		closest->atime_newnode = (1.0 - atime_imp) * closest->atime_newnode + atime_imp * dt;
	}
}

void GrowingNeuralGas::learnNode(vector<double> p_input, int p_id, unsigned int p_time)
{
	InputNode *node = 0;

	for(list<InputNode*>::iterator iter = m_inputnodes.begin(); iter != m_inputnodes.end(); iter++){
		if((*iter)->id == p_id){
			node = (*iter);
			break;
		}
	}

	if(node == 0){
		//id unknown!
		return;
	}

	Vertex *closest = adjustGNG(p_input);

	//Check if closest Vertex changed
	if(node->current != closest){

		//Check if connection exists
		VertexLink *link = 0;

		for(list<VertexLink*>::iterator iter = node->current->links.begin(); iter != node->current->links.end(); /* iter++ */){

			//Delete old connections
			if((*iter)->age > 50){
				delete (*iter);
				iter = node->current->links.erase(iter);
				continue;
			}

			//Age links
			(*iter)->age++;

			if((*iter)->link == closest){
				link = (*iter);
				//break;
			}

			iter++;
		}

		//Create link if nonexistant
		if(link == 0){
			link = new VertexLink(closest);
			node->current->links.push_back(link);

			link->atime_stay = node->since - p_time;

			link->last_transfer = p_time;
			node->since = p_time;
			node->current = closest;
		}

		//Update times if existant
		else{
			double atime_imp = 0.1;

			link->age = 0;

			//Stay of a node at closest vertex
			double dt = node->since - p_time;

			link->atime_stay_deviation = (1.0 - atime_imp) * link->atime_stay_deviation + atime_imp * (link->atime_stay - dt) * (link->atime_stay - dt);
			link->atime_stay = (1.0 - atime_imp) * link->atime_stay + atime_imp * dt;

			node->since = p_time;

			//Transfer rate of all nodes via this vertexlink
			dt = link->last_transfer - p_time;

			link->atime_transfer_deviation = (1.0 - atime_imp) * link->atime_transfer_deviation + atime_imp * (link->atime_transfer - dt) * (link->atime_transfer - dt);
			link->atime_transfer = (1.0 - atime_imp) * link->atime_transfer + atime_imp * dt;

			link->last_transfer = p_time;

			node->current = closest;
		}
	}
}

void GrowingNeuralGas::killNode(int p_id, unsigned int p_time)
{
	InputNode *node = 0;
	list<InputNode*>::iterator node_iter;

	for(list<InputNode*>::iterator iter = m_inputnodes.begin(); iter != m_inputnodes.end(); iter++){
		if((*iter)->id == p_id){
			node_iter = iter;
			node = (*iter);
			break;
		}
	}

	if(node == 0){
		//id unknown!
		return;
	}

	if(node->current->last_kill == 0){
		node->current->last_kill = p_time;
	}
	else{
		//Adjust closest kill rate
		double atime_imp = 0.1;

		double dt = p_time - node->since;

		node->current->atime_stay_kill_deviation = (1.0 - atime_imp) * node->current->atime_killnode_deviation + atime_imp * (node->current->atime_stay_kill - dt) * (node->current->atime_stay_kill - dt);
		node->current->atime_stay_kill = (1.0 - atime_imp) * node->current->atime_stay_kill + atime_imp * dt;

		dt = p_time - node->current->last_kill;

		node->current->atime_killnode_deviation = (1.0 - atime_imp) * node->current->atime_killnode_deviation + atime_imp * (node->current->atime_killnode - dt) * (node->current->atime_killnode - dt);
		node->current->atime_killnode = (1.0 - atime_imp) * node->current->atime_killnode + atime_imp * dt;

		node->current->last_kill = p_time;
	}

	//Erase node
	m_inputnodes.erase(node_iter);
	delete node;
}

Vertex* GrowingNeuralGas::adjustGNG(vector<double> p_input)
{
//	if(m_iterations == 200){
//		m_attraction_fact_first = 0.2;
//		m_attraction_fact_neighbors = 0.008;
//	}
//	m_attraction_fact_first *= 0.999;
//	m_attraction_fact_neighbors *= 0.999;

	//Find first (s1) and second (s2) closest vertex
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

		//s1->error_dim[a] = fabs(s1->position[a] - p_input[a]) * m_error_reduction_dim + (1.0 - m_error_reduction_dim) * s1->error_dim[a];
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
		if(m_age_edges != 0 && m_iterations != 0 && m_iterations % m_age_edges == 0){
			(*iter)->age++;
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

			delete (*iter);
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

		//Find vertex with highest error
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

		//Find neighbor with higest error and corresponding edge
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

		//Remove edge
		q->edges.remove(c);
		f->edges.remove(c);
		m_edges.remove(c);
		delete c;

		//Create vertex inbetween the found vertices
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

		//Create edges between found and new vertex
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

	return s1;
}

double GrowingNeuralGas::test_newNode(vector<double> p_input, int p_id, unsigned int p_time)
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

	double error_total = 0.0;
	double error;

	//Relative distance to input
	error = distmin / s1->getDistance(s2->position);
	error_total = accumulateError(error_total, error);

	if(s1->atime_newnode != 0.0){
		//Time (rate) of node creation at the closest vertex
		error = ((p_time - s1->last_new) - s1->atime_newnode) * ((p_time - s1->last_new) - s1->atime_newnode) / s1->atime_newnode_deviation / 2.0;
		error_total = accumulateError(error_total, error);
	}

	return error_total;
}

double GrowingNeuralGas::test_learnNode(vector<double> p_input, int p_id, unsigned int p_time)
{
	//Find corresponding inputnode
	InputNode *node = 0;

	for(list<InputNode*>::iterator iter = m_inputnodes.begin(); iter != m_inputnodes.end(); iter++){
		if((*iter)->id == p_id){
			node = (*iter);
			break;
		}
	}

	if(node == 0){
		//id unknown!
		return -1.0;
	}

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

	double error_total = 0.0;
	double error;

	//Relative distance to input
	error = distmin / s1->getDistance(s2->position);
	error_total = accumulateError(error_total, error);

	//Check if closest vertex changed
	if(node->current != s1){

		if(find(m_vertices.begin(), m_vertices.end(), node->current) == m_vertices.end()){
			node->current = s1;
			return error_total;
		}

		//Check for link from "current" to new vertex
		VertexLink *link = 0;

		for(list<VertexLink*>::iterator iter = node->current->links.begin(); iter != node->current->links.end(); iter++){
			if((*iter)->link == s1){
				link = (*iter);
				break;
			}
		}

		//Create link if nonexistant
		if(link == 0){
			//Add a lot of error
			error = 20.0;
			error_total = accumulateError(error_total, error);
		}
		else{
			//Duration of the stay of this node at the old vertex
			error = ((p_time - node->since) - link->atime_stay) * ((p_time - node->since) - link->atime_stay) / link->atime_stay_deviation / 2.0;
			error_total = accumulateError(error_total, error);

			//General transfer time of this link
			error = ((p_time - link->last_transfer) - link->atime_transfer) * ((p_time - link->last_transfer) - link->atime_transfer) / link->atime_transfer_deviation / 2.0;
			error_total = accumulateError(error_total, error);
		}
	}

	return error_total;
}

double GrowingNeuralGas::test_killNode(int p_id, unsigned int p_time)
{
	//Find corresponding inputnode
	InputNode *node = 0;

	for(list<InputNode*>::iterator iter = m_inputnodes.begin(); iter != m_inputnodes.end(); iter++){
		if((*iter)->id == p_id){
			node = (*iter);
			break;
		}
	}

	if(node == 0){
		//id unknown!
		return -1.0;
	}

	double error_total = 0.0;
	double error;

	//Stay of this node at the current vertex until death
	error = ((p_time - node->since) - node->current->atime_stay_kill) * ((p_time - node->since) - node->current->atime_stay_kill) / node->current->atime_stay_kill_deviation / 2.0;
	error_total = accumulateError(error_total, error);

	//General death time at this vertex
	error = ((p_time - node->current->last_kill) - node->current->atime_killnode) * ((p_time - node->current->last_kill) - node->current->atime_killnode) / node->current->atime_killnode_deviation / 2.0;
	error_total = accumulateError(error_total, error);

	return error_total;
}

double GrowingNeuralGas::accumulateError(double p_total, double p_component)
{
	//How the final error is computed
	return max(p_total, p_component); //Maybe not the smartest.. ToDo
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
