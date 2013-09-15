#ifndef GNUPLOT_OUTPUT_H
#define GNUPLOT_OUTPUT_H

#include <stdarg.h>
#include <QFile>

class GnuPlot_Output
{
public:
	GnuPlot_Output(QString p_filename);
	~GnuPlot_Output();
	void writeData(int p_args, ...);
	void flush();

private:
	QFile m_file;
};

#endif // GNUPLOT_OUTPUT_H
