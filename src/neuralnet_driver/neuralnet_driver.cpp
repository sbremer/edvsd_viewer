#include "neuralnet_driver.h"

NeuralNet_Driver::NeuralNet_Driver()
{
	const int size = 3;
	int layers[size] = {12, 3, 1};
	m_learnrate = 0.3;
	m_learnrate_reduction = 0.999;
	double rnd = 0.07; //0.04
	double rnd_bias = 2.0;

	m_nn = new NeuralNet(size, layers, 1, true);

	m_nn->initialize(rnd, rnd_bias);

	m_error = 0;
	m_performance = 1000;
}

NeuralNet_Driver::~NeuralNet_Driver()
{
	delete m_nn;
}

void NeuralNet_Driver::train(double t, double x)
{
	double performance_fact = 0.01;
	m_error = m_nn->train(&t, &x, m_learnrate);
	m_performance = m_performance * (1.0 - performance_fact) + m_error * performance_fact;
	m_learnrate *= m_learnrate_reduction;
}

double NeuralNet_Driver::calculate(double t)
{
	return m_nn->calculate(&t);
}

double NeuralNet_Driver::getLastError()
{
	return m_error;
}

double NeuralNet_Driver::getPerformance()
{
	return m_performance;
}
