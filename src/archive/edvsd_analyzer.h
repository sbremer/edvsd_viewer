#ifndef EDVSD_ANALYZER_H
#define EDVSD_ANALYZER_H

#include <vector>
#include <math.h>
#include <iostream>
#include <fstream>

#include "helper/eventf.h"

#include "startendtracker/startendtracker.h"
#include "kohonentracking/kohonentracking.h"
#include "neuralnet_driver/neuralnet_driver.h"

#include "output/gnuplot_output.h"

using namespace std;

class EDVSD_Analyzer
{
public:
	EDVSD_Analyzer(vector<double> p_param);
	double analyze(EventF *p_buffer, int p_n);

private:

	StartEndTracker m_startendtracker;
	KohonenTracking<2> m_kohonentracker;
	//NeuralNet_Driver m_neuralnet_x;
	//NeuralNet_Driver m_neuralnet_y;

	GnuPlot_Output m_output;
};

#endif // EDVSD_ANALYZER_H
