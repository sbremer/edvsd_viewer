#include "edvsd_analyzer.h"

EDVSD_Analyzer::EDVSD_Analyzer(vector<double> p_param)
	:m_startendtracker(p_param[0], p_param[1], p_param[2], p_param[3], p_param[4]),
	  m_kohonentracker(p_param[5], p_param[6], p_param[7], p_param[8], p_param[9], p_param[10]),
	  m_output("data.dat")
	  //m_neuralnet_x(p_param[11], p_param[12], p_param[13], p_param[14], p_param[15]),
	  //m_neuralnet_y(p_param[11], p_param[12], p_param[13], p_param[14], p_param[15])
	  //m_neuralnet_x(),
	  //m_neuralnet_y()
{
//	m_startendtracker = StartEndTracker(p_param[0], p_param[1], p_param[2], p_param[3], p_param[4]);
//	m_kohonentracker = KohonenTracking<2>(p_param[5], p_param[6], p_param[7], p_param[8], p_param[9], p_param[10]);
//	m_neuralnet_x = NeuralNet_Driver(p_param[11], p_param[12], p_param[13], p_param[14], p_param[15]);
//	m_neuralnet_y = NeuralNet_Driver(p_param[11], p_param[12], p_param[13], p_param[14], p_param[15]);
}

double EDVSD_Analyzer::analyze(EventF *p_buffer, int p_n)
{
	list<MotionF> motions = m_startendtracker.trackPoints(p_buffer, p_n);

	int time_comp = -1;

	m_kohonentracker.initialize(motions.begin()->start, motions.begin()->end, true);

	int writeerror = -1;

	for(int a = 0; a < p_n; a++){
		const KohonenMap<2>* map = m_kohonentracker.analyzeEvent(p_buffer[a]);

		if(map == 0){
			continue;
		}

		if(m_kohonentracker.getDurationMin() < 1000000 && time_comp == -1){
			time_comp = m_kohonentracker.getDurationMin();
			writeerror = a;
		}

		if(time_comp != -1 && map->ts != -1){
			double x = (map->points[0].x + map->points[1].x) / 2.0;
			double y = (map->points[0].y + map->points[1].y) / 2.0;
			double angle = atan((map->points[0].x - map->points[1].x) / (map->points[0].y - map->points[1].y));
			double t = (double)p_buffer[a].ts - map->ts;

			x = (x - 64) / 64;
			y = (y - 64) / 64;
			angle = angle;
			t = (t - time_comp / 2.0) / (time_comp / 4.0);

			m_output.writeData(3, t, x, y);
			//m_neuralnet_x.train(t, x);
			//m_neuralnet_y.train(t, y);
			//m_neuralnet_atan.train(t, angle);
		}
	}
	m_output.flush();
	int an = system("gnuplot -e \"load 'plot_xy.plt';\" > log 2>&1");


	FILE * file = popen("grep \"final sum of squares of residuals :\" log", "r");
	//fstream stream(file);
	//char buffer[256];
	//fgets(buffer, 128, file);


	char *ptr = NULL;
	size_t len;
	double a = 0;
	double sum = 0;
	while(getline(&ptr, &len, file) != EOF){
		//cout << "Buffer: \"" << ptr + 36 << "\"" << endl;
		//ptr[len-1] = 0;
		ptr[37] = ',';
		a = (double)atof((ptr + 36));
		//cout << "Val: " << a << endl;
		sum += a;
		//36
	}

	//grep "final sum of squares of residuals :" log
	//cout << "Sum: " << sum << endl;
	return 1000 / sum;
	//return 1000.0 / (m_neuralnet_x.getPerformance() + m_neuralnet_y.getPerformance());
}
