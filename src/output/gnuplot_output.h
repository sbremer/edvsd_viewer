#ifndef GNUPLOT_OUTPUT_H
#define GNUPLOT_OUTPUT_H

#include <vector>
#include <stdarg.h>
#include <QFile>

using namespace std;

class GnuPlot_Output
{
public:
	GnuPlot_Output(QString p_filename);
	~GnuPlot_Output();
	void writeData(int p_args, ...);
	void writeData(const vector<double> &p_data);
	void clear();
	void flush();

private:
	QFile* m_file;
};

#endif // GNUPLOT_OUTPUT_H
