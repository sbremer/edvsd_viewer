#include "edvsd_fileprocessor.h"

EDVSD_FileProcessor::EDVSD_FileProcessor(QObject *parent) :
	QObject(parent)
{
	m_file = NULL;
	m_fileopen = false;
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
int EDVSD_FileProcessor::getSizeX()
{
	return m_size_x;
}

int EDVSD_FileProcessor::getSizeY()
{
	return m_size_y;
}

EDVS_Timestamp_Resolution EDVSD_FileProcessor::getTimestampResolution()
{
	return m_timestampresolution;
}

bool EDVSD_FileProcessor::loadFile(QString p_filename)
{
	if(m_fileopen)
		return false;
	m_filename = p_filename;
	if(m_filename.length()>0&&QFile(m_filename).exists()){
		m_file = new QFile(m_filename);
		if(!m_file->open(QIODevice::ReadOnly))
			return false;
		char buffer[5];
		if(m_file->read(buffer,4)!=4)
			return false;

		//File Version can be different to 1 in the future!
		if(buffer[0]!=1)
			return false;
		m_size_x = (int)buffer[1];
		m_size_y = (int)buffer[2];
		m_timestampresolution = (EDVS_Timestamp_Resolution)buffer[3];
		if(m_timestampresolution<0||m_timestampresolution>3)
			return false;
		m_fileopen = true;
		return true;
	}
}

void EDVSD_FileProcessor::closeFile()
{
	if(!m_fileopen)
		return;
	if(m_file!=NULL)
		delete m_file;
	m_fileopen = false;
	m_filename = "";
}

int EDVSD_FileProcessor::readEvents(int p_n)
{
	int read = 0;
	EDVS_Event buffer[EDVSD_BUFFER_SIZE];
	int i;
	while(read<p_n){
		i = m_file->read((char*)buffer, sizeof(EDVS_Event)*(p_n-read<EDVSD_BUFFER_SIZE ? p_n-read : EDVSD_BUFFER_SIZE));
		if(i==-1)
			break;
		else if(i<EDVSD_BUFFER_SIZE*sizeof(EDVS_Event)){
			emit eventsRead(buffer, i/sizeof(EDVS_Event));
			read+=i/sizeof(EDVS_Event);
			break;
		}
		else{
			emit eventsRead(buffer, EDVSD_BUFFER_SIZE);
			read+=EDVSD_BUFFER_SIZE;
		}
	}
	return read;
}

int EDVSD_FileProcessor::readEventsByTime(int p_t)
{

}
