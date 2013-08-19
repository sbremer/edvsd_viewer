#include "edvsd_fileprocessor.h"

EDVSD_FileProcessor::EDVSD_FileProcessor(QObject *parent) :
	QObject(parent)
{
	m_file = NULL;
}

EDVSD_FileProcessor::~EDVSD_FileProcessor()
{
	if(m_file!=NULL)
		delete m_file;
}

QString EDVSD_FileProcessor::getFileName()
{
	return m_filename;
}

bool EDVSD_FileProcessor::loadFile(QString p_filename)
{
	m_filename = p_filename;
	if(m_filename.length()>0&&QFile(m_filename).exists()){
		m_file = new QFile(m_filename);
		if(!m_file->open(QIODevice::ReadOnly))return false;
		char buffer[5];
		if(m_file->read(buffer,4)!=4)return false;

		//File Version can be different to 1 in the future!
		if(buffer[0]!=1)return false;


	}
}
