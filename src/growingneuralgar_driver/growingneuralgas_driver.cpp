#include "growingneuralgas_driver.h"

GrowingNeuralGas_Driver::GrowingNeuralGas_Driver(int p_dim)
	:m_gng(p_dim), m_learning(true)
{

}

double GrowingNeuralGas_Driver::learn_node(vector<double> p_input, int p_id, unsigned int p_time)
{
	//Doing both in one step, minimizing overlaping code would be more efficient!
	double error = m_gng.test_learnNode(p_input, p_id, p_time);
	if(m_learning)m_gng.learnNode(p_input, p_id, p_time);
	return error;
}

double GrowingNeuralGas_Driver::new_node(vector<double> p_input, int p_id, unsigned int p_time)
{
	//Doing both in one step, minimizing overlaping code would be more efficient!
	double error = m_gng.test_newNode(p_input, p_id, p_time);
	if(m_learning)m_gng.newNode(p_input, p_id, p_time);
	return error;
}

double GrowingNeuralGas_Driver::kill_node(int p_id, unsigned int p_time)
{
	//Doing both in one step, minimizing overlaping code would be more efficient!
	double error = m_gng.test_killNode(p_id, p_time);
	if(m_learning)m_gng.killNode(p_id, p_time);
	return error;
}

void GrowingNeuralGas_Driver::setLearning(bool p_learning)
{
	m_learning = p_learning;
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
