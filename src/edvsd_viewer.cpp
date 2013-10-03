#include "edvsd_viewer.h"
#include "ui_edvsd_viewer.h"

#include "iostream"
using namespace std;

EDVSD_Viewer::EDVSD_Viewer(QWidget *parent) :
    QMainWindow(parent),
	m_ui(new Ui::EDVSD_Viewer)
{
	m_ui->setupUi(this);
	m_fileprocessor = new EDVSD_FileProcessor();
	m_visualizer = NULL;
	m_detection = NULL;
}

EDVSD_Viewer::~EDVSD_Viewer()
{
	delete m_ui;
	delete m_fileprocessor;
	if(m_visualizer!=NULL)
		delete m_visualizer;
	if(m_detection!=NULL)
		delete m_detection;
}

void EDVSD_Viewer::loadEventData()
{
	//Todo: Add feature for manipolation of replay speed
	int c = m_fileprocessor->readEventsByTime(33*30);
	//int c = m_fileprocessor->readEvents(10);
//	if(c!=0)
//		cout << c << endl;
}

void EDVSD_Viewer::on_actionE_xit_triggered()
{
	this->close();
}

void EDVSD_Viewer::on_action_Close_File_triggered()
{
	m_fileprocessor->closeFile();
	if(m_visualizer!=NULL){
		disconnect(m_fileprocessor, SIGNAL(eventsRead(EDVS_Event*,int)), m_visualizer, SLOT(drawEvents(EDVS_Event*,int)));
		disconnect(m_visualizer, SIGNAL(loadEventData()), this, SLOT(loadEventData()));
		delete m_visualizer;
		m_visualizer = NULL;
	}
	if(m_detection!=NULL){
		disconnect(m_fileprocessor, SIGNAL(eventsRead(EDVS_Event*,int)), m_detection, SLOT(analyzeLiveEvents(EDVS_Event*,int)));
		delete m_detection;
		m_detection = NULL;
	}
}

void EDVSD_Viewer::on_action_Open_File_triggered()
{
	QString filename = QFileDialog::getOpenFileName(this, "Load dvsd file", "", "*.dvsd");
	if(filename.length()<1)return;
	on_action_Close_File_triggered();
	if(m_fileprocessor->loadFile(filename)){
		m_visualizer = new EDVSD_Visualizer(this, m_fileprocessor->getSizeX(), m_fileprocessor->getSizeY());
		connect(m_fileprocessor, SIGNAL(eventsRead(EDVS_Event*,int)), m_visualizer, SLOT(drawEvents(EDVS_Event*,int)));
		connect(m_visualizer, SIGNAL(loadEventData()), this, SLOT(loadEventData()));
		m_detection = new EDVSD_Anormaly_Detection();
		m_detection->setDebugPainter(m_visualizer->getDebugPainter());

		connect(m_fileprocessor, SIGNAL(eventsRead(EDVS_Event*,int)), m_detection, SLOT(analyzeLiveEvents(EDVS_Event*,int)));

		m_ui->statusBar->showMessage("\nFile: " + m_fileprocessor->getFileName()
				+ " SizeX: " + QString::number((int)(m_fileprocessor->getSizeX()))
				+ " SizeY: " + QString::number((int)(m_fileprocessor->getSizeY()))
				+ " TSRes(in byte): " + QString::number((int)(m_fileprocessor->getTimestampResolution()!=0? m_fileprocessor->getTimestampResolution() + 1 : 0))
				+ " TE: " + QString::number(m_fileprocessor->getTotalEvents()));
		m_visualizer->move(0,26);
		m_visualizer->setScaler(5.0);
		if(m_ui->action_White_Black->isChecked())m_visualizer->setMode(EDVS_Visualization_Mode_White_Black);
		m_visualizer->show();

		m_detection->analyzeEvents(m_fileprocessor->getEventPtr(), m_fileprocessor->getTotalEvents());


//		QFile output(filename+".txt");
//		output.open(QIODevice::WriteOnly);
//		EDVS_Event* eventptr = m_fileprocessor->getEventPtr();
//		for(int a=0;a<m_fileprocessor->getTotalEvents();a++){
//			QString data = QString::number(eventptr[a].t) + "\t" + QString::number(eventptr[a].x) + "\t" + QString::number(eventptr[a].y) + "\t" + QString::number(eventptr[a].p) + "\n";
//			output.write(data.toLocal8Bit().data());
//		}
//		output.close();

		m_visualizer->start();
	}
	else{
		m_ui->statusBar->showMessage("\nUnable to open " + filename + "!");
	}
}

void EDVSD_Viewer::on_action_Green_Red_triggered()
{
	m_ui->action_White_Black->setChecked(false);
	m_ui->action_White_Black->setEnabled(true);
	m_ui->action_Green_Red->setEnabled(false);
	if(m_visualizer!=NULL)
		m_visualizer->setMode(EDVS_Visualization_Mode_Green_Red);
}

void EDVSD_Viewer::on_action_White_Black_triggered()
{
	m_ui->action_Green_Red->setChecked(false);
	m_ui->action_Green_Red->setEnabled(true);
	m_ui->action_White_Black->setEnabled(false);
	if(m_visualizer!=NULL)
		m_visualizer->setMode(EDVS_Visualization_Mode_White_Black);
}

void EDVSD_Viewer::on_actionDump_NNData_triggered()
{
	if(m_detection != NULL)
		m_detection->dumpNNData();
}
