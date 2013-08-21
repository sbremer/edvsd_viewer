#include "edvsd_fileprocessor.h"

#include "iostream"
using namespace std;

EDVSD_FileProcessor::EDVSD_FileProcessor(QObject *parent) :
	QObject(parent)
{
	m_file = NULL;
	m_fileopen = false;
	m_timestamp = 0;
	m_totalevents = 0;
	m_pos = 0;
}

EDVSD_FileProcessor::~EDVSD_FileProcessor()
{
	if(m_file!=NULL)
		delete m_file;
	m_data.clear();
}

QString EDVSD_FileProcessor::getFileName()
{
	if(!m_fileopen)return QString("");
	return m_file->fileName();
}
unsigned int EDVSD_FileProcessor::getSizeX()
{
	if(!m_fileopen)return 0;
	return m_size_x;
}

unsigned int EDVSD_FileProcessor::getSizeY()
{
	if(!m_fileopen)return 0;
	return m_size_y;
}

EDVS_Timestamp_Resolution EDVSD_FileProcessor::getTimestampResolution()
{
	if(!m_fileopen)return EDVS_Timestamp_Resolution_None;
	return m_timestampresolution;
}

int EDVSD_FileProcessor::getTotalEvents()
{
	if(!m_fileopen)return 0;
	return m_totalevents;
}

bool EDVSD_FileProcessor::loadFile(QString p_filename)
{
	if(m_fileopen)
		return false;
	if(p_filename.length()>0&&QFile(p_filename).exists()){
		m_file = new QFile(p_filename);
		if(!m_file->open(QIODevice::ReadOnly))
			return false;
		unsigned char buffer[5];
		if(m_file->read((char*)buffer,4)!=4)
			return false;

		//File Version can be different to 1 in the future!
		if(buffer[0]!=1)
			return false;
		m_size_x = (unsigned int)buffer[1];
		m_size_y = (unsigned int)buffer[2];
		m_timestampresolution = (EDVS_Timestamp_Resolution)buffer[3];
		if(m_timestampresolution<0||m_timestampresolution>3)
			return false;
		m_file->seek(0);
		m_data = m_file->readAll();
		if(m_data.length()<4+sizeof(EDVS_Event))
			return false;
		m_eventptr = (EDVS_Event*)(m_data.data()+4);
		m_totalevents = (m_data.length()-4)/sizeof(EDVS_Event);
		m_pos = 0;
		m_timestamp = m_eventptr[0].t;
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
	m_timestamp = 0;
	m_data.clear();
	m_totalevents = 0;
}

int EDVSD_FileProcessor::readEvents(unsigned int p_n)
{
	if(m_totalevents==m_pos){
		return 0;
	}
	else if(m_totalevents-m_pos<p_n){
		emit eventsRead(&(m_eventptr[m_pos]), m_totalevents-m_pos);
		m_pos = m_totalevents;
		return m_totalevents-m_pos;
	}
	else{
		emit eventsRead(&(m_eventptr[m_pos]), p_n);
		m_pos+=p_n;
		return p_n;
	}
}

int EDVSD_FileProcessor::readEventsByTime(quint32 p_t)
{
	int read = 0;
	quint32 timestamp_new;
	switch(m_timestampresolution){
	case EDVS_Timestamp_Resolution_16bit:
		timestamp_new = 0xFFFF & (m_timestamp + p_t);
		break;
	case EDVS_Timestamp_Resolution_24bit:
		timestamp_new = 0xFFFFFF & (m_timestamp + p_t);
		break;
	case EDVS_Timestamp_Resolution_32bit:
		timestamp_new = 0xFFFFFFFF & (m_timestamp + p_t);
		break;
	case EDVS_Timestamp_Resolution_None:
	default:
		return 0;
	}
	int a = m_pos;

	cout << timestamp_new << endl;

	//Detect overflow
	if(timestamp_new<m_timestamp){
		while(a<m_totalevents && m_eventptr[a].t>timestamp_new){
			a++;
		}
	}

	while(a<m_totalevents && m_eventptr[a].t<timestamp_new){
		a++;
	}
	read = a-m_pos;
	emit eventsRead(&(m_eventptr[m_pos]), read);
	m_pos=a;
	m_timestamp = timestamp_new;
	return read;
}
