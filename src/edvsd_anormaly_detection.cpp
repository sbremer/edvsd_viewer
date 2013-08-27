#include "edvsd_anormaly_detection.h"

EDVSD_Anormaly_Detection::EDVSD_Anormaly_Detection(QObject *parent) :
	QObject(parent)
{
}

void EDVSD_Anormaly_Detection::setDebugPainter(QPainter *p_painter)
{
	m_painter = p_painter;
}

void EDVSD_Anormaly_Detection::analyzeEvents(EDVS_Event *p_buffer, int p_n)
{

	m_painter->drawLine(100,100,101,101);

}
