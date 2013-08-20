#ifndef EDVSD_FILEPROCESSOR_H
#define EDVSD_FILEPROCESSOR_H

#include <QObject>
#include <QFile>

#include "edvsd.h"

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
	int getSizeX();
	int getSizeY();
	EDVS_Timestamp_Resolution getTimestampResolution();

	int readEvents(int p_n);
	int readEventsByTime(int p_t);

private:
	QString m_filename;
	QFile *m_file;

	bool m_fileopen;
	int m_size_x, m_size_y;
	EDVS_Timestamp_Resolution m_timestampresolution;
	
signals:
	void eventsRead(EDVS_Event *p_buffer, int p_n);
	
public slots:
	
};

#endif // EDVSD_FILEPROCESSOR_H
