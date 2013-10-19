#include "growingneuralgas_driver.h"

GrowingNeuralGas_Driver::GrowingNeuralGas_Driver(int p_dim)
	:m_gng(p_dim)
{

}

void GrowingNeuralGas_Driver::learn(vector<double> p_input)
{
	m_gng.learn(p_input);
}

void GrowingNeuralGas_Driver::dumpData()
{
	GnuPlot_Output output("gng.dat");

	const list<Vertex*> &vertices = m_gng.getVertices();

	for(list<Vertex*>::const_iterator iter = vertices.begin(); iter != vertices.end(); iter++){
		output.writeData(2, (*iter)->position[0], (*iter)->position[1]);
	}

	output.flush();
}
