#ifndef NORMALIZER_H
#define NORMALIZER_H

#include <math.h>
#include <vector>
using namespace std;

class Normalizer
{
public:
	Normalizer(int p_dim);
	vector<double> normalize(vector<double> p_input);

private:
	const int m_dim;
	double m_learnrate;
	double m_learnratereduction;
	vector<double> m_mean;
	vector<double> m_deviation;
};

#endif // NORMALIZER_H
