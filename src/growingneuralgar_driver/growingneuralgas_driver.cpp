#include "growingneuralgas_driver.h"

GrowingNeuralGas_Driver::GrowingNeuralGas_Driver(int p_dim)
	:m_gng(p_dim)
{

}

void GrowingNeuralGas_Driver::learn(vector<double> p_input)
{
	m_gng.learnNode(p_input, 0, 0);
}

double GrowingNeuralGas_Driver::test(vector<double> p_input)
{
	return m_gng.testNode(p_input, 0, 0);
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
		vector<double> data;
//		for(int a = 0; a < m_gng.getDimension(); a++){
//			data.push_back((*iter)->vertex1->position[a]);
//		}
//		for(int a = 0; a < m_gng.getDimension(); a++){
//			data.push_back((*iter)->vertex2->position[a]);
//		}
		output2.writeData((*iter)->vertex1->position);
		output2.writeData((*iter)->vertex2->position);
		output2.writeData(0);
		output2.writeData(0);
	}
}
