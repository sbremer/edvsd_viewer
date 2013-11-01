#include "gnuplot_output.h"

GnuPlot_Output::GnuPlot_Output(QString p_filename)
{
	//system("rm "+p_filename);
	m_file.setFileName(p_filename);
	m_file.remove();
	m_file.open(QIODevice::WriteOnly);
}

GnuPlot_Output::~GnuPlot_Output()
{
	m_file.flush();
	m_file.close();
}

void GnuPlot_Output::writeData(int p_args, ...)
{
	va_list ap;
	va_start(ap, p_args);
	QString data;
	for(int a = 0; a < p_args; a++){
		data += QString::number(va_arg(ap, double)) + "\t";
	}
	va_end(ap);
	data += "\n";

	m_file.write(data.toLocal8Bit().data());
	//m_file.flush();
}

void GnuPlot_Output::writeData(const vector<double> &p_data)
{
	QString data;
	for(int a = 0; a < p_data.size(); a++){
		data += QString::number(p_data[a]) + "\t";
	}
	data += "\n";
	m_file.write(data.toLocal8Bit().data());
}

void GnuPlot_Output::clear()
{
	m_file.close();
	m_file.remove();
	m_file.open(QIODevice::WriteOnly);
}

void GnuPlot_Output::flush()
{
	m_file.flush();
}
