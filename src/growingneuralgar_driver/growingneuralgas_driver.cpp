#include "growingneuralgas_driver.h"

#include <iostream>
using namespace std;

//	p_max_age, p_generate_neuron, p_max_vertices, p_age_edges);


GrowingNeuralGas_Driver::GrowingNeuralGas_Driver(int p_dim)
	:m_gng(p_dim, 0.15, 0.02, 20, 20, 200, 100), m_normalizer(p_dim), m_gng_newkill(3, 0.5, 0.05, 5, 3, 30, 4), m_normalizer_newkill(2), m_learning(true)
{

}

double GrowingNeuralGas_Driver::processFeatureEvent(FeatureEvent p_featureevent)
{
	switch(p_featureevent.type){
	case FEATURE_EVENT_TYPE_NEW_NODE:
		return new_node(p_featureevent.features, p_featureevent.id, p_featureevent.time);
	case FEATURE_EVENT_TYPE_LEARN_NODE:
		return learn_node(p_featureevent.features, p_featureevent.id, p_featureevent.time);
	case FEATURE_EVENT_TYPE_KILL_NODE:
		return kill_node(p_featureevent.features, p_featureevent.id, p_featureevent.time);
	default:
		//Error
		return 0.0;
	}
}

double GrowingNeuralGas_Driver::learn_node(vector<double> p_input, int p_id, unsigned int p_time)
{
	//Normalize inputs
	p_input = m_normalizer.normalize(p_input);

	//Test and learn input with GNG
	double error = m_gng.test(p_input);
	if(m_learning)m_gng.learn(p_input);
	return error;
}

double GrowingNeuralGas_Driver::new_node(vector<double> p_input, int p_id, unsigned int p_time)
{
	//Normalize inputs
	p_input = m_normalizer_newkill.normalize(p_input);

	//Data for "New" Tracker
	p_input.push_back(3.0);

	//Test and learn input with GNG
	double error = m_gng_newkill.test(p_input);
	if(m_learning)m_gng_newkill.learn(p_input);
	return error;
}

double GrowingNeuralGas_Driver::kill_node(vector<double> p_input, int p_id, unsigned int p_time)
{
	//Normalize inputs
	p_input = m_normalizer_newkill.normalize(p_input);

	//Data for "Dead" Tracker
	p_input.push_back(-3.0);

	//Test and learn input with GNG
	double error = m_gng_newkill.test(p_input);
	if(m_learning)m_gng_newkill.learn(p_input);
	return error;
}

void GrowingNeuralGas_Driver::setLearning(bool p_learning)
{
	m_learning = p_learning;
	m_normalizer.setLearning(p_learning);
	m_normalizer_newkill.setLearning(p_learning);
}

void GrowingNeuralGas_Driver::dumpData()
{
	GnuPlot_Output output("gng.dat");

	const list<Vertex*> &vertices = m_gng.getVertices();

	for(list<Vertex*>::const_iterator iter = vertices.begin(); iter != vertices.end(); iter++){
		output.writeData((*iter)->position);
	}

	output.flush();

	GnuPlot_Output output2("gnge.dat");

	const list<Edge*> &edges = m_gng.getEdges();

	for(list<Edge*>::const_iterator iter = edges.begin(); iter != edges.end(); iter++){
		output2.writeData((*iter)->vertex1->position);
		output2.writeData((*iter)->vertex2->position);
		output2.writeData(0);
		output2.writeData(0);
	}

	output2.flush();
}
