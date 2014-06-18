#ifndef EDVSD_FILEPROCESSOR_H
#define EDVSD_FILEPROCESSOR_H

#include <QObject>
#include <QFile>

#include <vector>

#include "helper/random.h"
#include "helper/eventf.h"

#include "edvsd.h"

using namespace std;

#define EDVSD_BUFFER_SIZE 512

class EDVSD_FileProcessor : public QObject
{
	Q_OBJECT
public:
	explicit EDVSD_FileProcessor(QObject *parent = 0);
	~EDVSD_FileProcessor();
	bool loadFile(QString p_filename);
	void closeFile();

	QString getFileName();
	unsigned int getSizeX();
	unsigned int getSizeY();
	EDVS_Timestamp_Resolution getTimestampResolution();

	EDVS_Event *getEventPtr();
	EventF *getEventPtrF();
	int getTotalEvents();
	int readEvents(unsigned int p_n);
	int readEventsByTime(quint32 p_t);

	quint32 getCurrentTimestamp();

private:
	QFile *m_file;

	QByteArray m_data;
	EDVS_Event *m_eventptr;
	int m_totalevents;
	int m_pos;

	bool m_fileopen;
	unsigned int m_size_x, m_size_y;
	EDVS_Timestamp_Resolution m_timestampresolution;

	vector<EventF> m_eventbuffer;

	quint32 m_timestamp;
	
signals:
	void eventsRead(EDVS_Event *p_buffer, int p_n);
	void eventsReadF(EventF *p_buffer, int p_n);
	
public slots:
	
};

#endif // EDVSD_FILEPROCESSOR_H
