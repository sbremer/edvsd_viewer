#ifndef NEURALNET_FUNCTIONS_H
#define NEURALNET_FUNCTIONS_H

#include "math.h"

inline double tanh_deriv(double x)
{
	return (1.0/cosh(x)/cosh(x));
}

inline double linear(double x)
{
	return x;
}

inline double linear_deriv(double x)
{
	return 1.0;
}

typedef double (*TransferFunction)(double);

struct ActivationFunction{
	ActivationFunction(TransferFunction p_function, TransferFunction p_function_derivative)
		:m_function(p_function), m_function_derivative(p_function_derivative)
	{

	}

	const TransferFunction m_function;
	const TransferFunction m_function_derivative;
};

const struct ActivationFunction Function_Tanh (tanh, tanh_deriv);

const struct ActivationFunction Function_Linear (linear, linear_deriv);

#endif // NEURALNET_FUNCTIONS_H
