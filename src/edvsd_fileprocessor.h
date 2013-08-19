#ifndef EDVSD_FILEPROCESSOR_H
#define EDVSD_FILEPROCESSOR_H

#include <QObject>
#include <QFile>

class EDVSD_FileProcessor : public QObject
{
	Q_OBJECT
public:
	explicit EDVSD_FileProcessor(QObject *parent = 0);
	~EDVSD_FileProcessor();
	bool loadFile(QString p_filename);
	QString getFileName();

private:
	QString m_filename;
	QFile *m_file;
	
signals:
	
public slots:
	
};

#endif // EDVSD_FILEPROCESSOR_H
