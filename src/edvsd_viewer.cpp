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

	m_tracking_param.resize(11);

    m_tracking_param[0] = 2.29423;
    m_tracking_param[1] = 4.86432;
    m_tracking_param[2] = 0.85372;
    m_tracking_param[3] = 1.6532;
    m_tracking_param[4] = 4.82759;
    m_tracking_param[5] = 4.01561;
    m_tracking_param[6] = 4.84066;
	m_tracking_param[7] = 2.5;//1.94633;
    m_tracking_param[8] = 3.96409;
    m_tracking_param[9] = 0.133502;
    m_tracking_param[10] = 0.0117267;

	//		tracking_param[0] = 2.5; //StartEndTracker::m_attraction_fact_start
	//		tracking_param[1] = 3.2; //StartEndTracker::m_attraction_pow_start
	//		tracking_param[2] = 1.0; //StartEndTracker::m_dist
	//		tracking_param[3] = 2.0; //StartEndTracker::m_attraction_fact_end
	//		tracking_param[4] = 3.3; //StartEndTracker::m_attraction_pow_end

	//		tracking_param[5] = 4.0; //KohonenTracking::m_start_dist
	//		tracking_param[6] = 4.0; //KohonenTracking::m_end_dist
	//		tracking_param[7] = 3.0; //KohonenTracking::m_attraction_fact
	//		tracking_param[8] = 3.0; //KohonenTracking::m_attraction_pow
	//		tracking_param[9] = 0.3; //KohonenTracking::m_attraction_max
	//		tracking_param[10] = 0.01; //KohonenTracking::m_neighbor_attraction

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
	int c = m_fileprocessor->readEventsByTime(33*15);
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

		m_detection = new EDVSD_Anormaly_Detection();//NULL, m_tracking_param);
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

void EDVSD_Viewer::on_action_GA_triggered()
{
	EDVS_Event *buffer_raw = m_fileprocessor->getEventPtr();
	vector<EventF> buffer;
	buffer.resize(m_fileprocessor->getTotalEvents());

	for(int a = 0; a < m_fileprocessor->getTotalEvents(); a++){
		buffer[a] = EventF(buffer_raw[a].x, buffer_raw[a].y, buffer_raw[a].p, buffer_raw[a].t);
	}

	GeneticAlgorithm_Driver driver(&(buffer[0]), m_fileprocessor->getTotalEvents(), 0.5);
	m_tracking_param = driver.runGeneticAlgorithm();
}

void EDVSD_Viewer::on_action_Testopen_File_triggered()
{
	QString filename = QFileDialog::getOpenFileName(this, "Load dvsd file", "", "*.dvsd");
	if(filename.length()<1)return;
	m_fileprocessor->closeFile();
	if(m_fileprocessor->loadFile(filename)){

		m_detection->testEvents(m_fileprocessor->getEventPtr(), m_fileprocessor->getTotalEvents());

		disconnect(m_fileprocessor, SIGNAL(eventsRead(EDVS_Event*,int)), m_detection, SLOT(analyzeLiveEvents(EDVS_Event*,int)));
		disconnect(m_fileprocessor, SIGNAL(eventsRead(EDVS_Event*,int)), m_detection, SLOT(testLiveEvents(EDVS_Event*,int)));
		connect(m_fileprocessor, SIGNAL(eventsRead(EDVS_Event*,int)), m_detection, SLOT(testLiveEvents(EDVS_Event*,int)));
	}
	else{
		m_ui->statusBar->showMessage("\nUnable to open " + filename + "!");
	}
}























