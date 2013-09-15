#ifndef NEURALNET_DRIVER_H
#define NEURALNET_DRIVER_H

#include "neuralnet/neuralnet.h"
#include "output/gnuplot_output.h"

class NeuralNet_Driver
{
public:
	NeuralNet_Driver();
	~NeuralNet_Driver();

	void train(double t, double x);
	double calculate(double t);
	double getLastError();
	double getPerformance();

private:
	NeuralNet *m_nn;
	double m_learnrate;
	double m_learnrate_reduction;
	double m_error;
	double m_performance;
};

#endif // NEURALNET_DRIVER_H
