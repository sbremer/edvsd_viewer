#ifndef EDVSD_ANORMALY_DETECTION_H
#define EDVSD_ANORMALY_DETECTION_H

#define QT

#include <QPainter>
#include <qmath.h>

#include "edvsd.h"
#include "helper/pointf.h"
#include "growingneuralgar_driver/growingneuralgas_driver.h"

#include "dyntracker/dyntracker.h"

struct ErrorCollector{
	ErrorCollector()
		:error(0.0), n(0)
	{

	}

	double error;
	int n;
};

class EDVSD_Anormaly_Detection : public QObject
{
	Q_OBJECT

public:
	explicit EDVSD_Anormaly_Detection(QObject *parent = 0);
	~EDVSD_Anormaly_Detection();

	void setDebugPainter(QPainter *p_painter);

	void analyzeEvents(EventF *p_buffer, int p_n);
	void testEvents(EventF *p_buffer, int p_n);

public slots:
	void analyzeLiveEvents(EventF *p_buffer, int p_n);
	void testLiveEvents(EventF *p_buffer, int p_n);

private:
	QPainter *m_painter;

    DynTracker m_dyntracker;

	GrowingNeuralGas_Driver m_gngd;

	ErrorCollector **m_collectors;

	double m_error;
};

#endif // EDVSD_ANORMALY_DETECTION_H
