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

	on_action1_64x_triggered();

	m_tracking_param.resize(11);

	m_tracking_param[0] = 1.36506;
	m_tracking_param[1] = 3.24748;
	m_tracking_param[2] = 0.974199;
	m_tracking_param[3] = 1.50172;
	m_tracking_param[4] = 4.37303;
	m_tracking_param[5] = 2.44994;
	m_tracking_param[6] = 2.43144;
	m_tracking_param[7] = 1.82968;
	m_tracking_param[8] = 4.27747;
	m_tracking_param[9] = 0.159401;
	m_tracking_param[10] = 0.00850496;

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
	int c = m_fileprocessor->readEventsByTime(m_replay_speed);
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
		disconnect(m_fileprocessor, SIGNAL(eventsReadF(EventF*,int)), m_detection, SLOT(analyzeLiveEvents(EventF*,int)));
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

		connect(m_fileprocessor, SIGNAL(eventsReadF(EventF*,int)), m_detection, SLOT(analyzeLiveEvents(EventF*,int)));

		m_ui->statusBar->showMessage("\nFile: " + m_fileprocessor->getFileName()
				+ " SizeX: " + QString::number((int)(m_fileprocessor->getSizeX()))
				+ " SizeY: " + QString::number((int)(m_fileprocessor->getSizeY()))
				+ " TSRes(in byte): " + QString::number((int)(m_fileprocessor->getTimestampResolution()!=0? m_fileprocessor->getTimestampResolution() + 1 : 0))
				+ " TE: " + QString::number(m_fileprocessor->getTotalEvents()));
		m_visualizer->move(0,26);
		m_visualizer->setScaler(5.0);
		if(m_ui->action_White_Black->isChecked())m_visualizer->setMode(EDVS_Visualization_Mode_White_Black);
		m_visualizer->show();

		m_detection->analyzeEvents(m_fileprocessor->getEventPtrF(), m_fileprocessor->getTotalEvents());


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
	GeneticAlgorithm_Driver driver(m_fileprocessor->getEventPtrF(), m_fileprocessor->getTotalEvents(), 0.5);
	m_tracking_param = driver.runGeneticAlgorithm();
}

void EDVSD_Viewer::on_action_Testopen_File_triggered()
{
	QString filename = QFileDialog::getOpenFileName(this, "Load dvsd file", "", "*.dvsd");
	if(filename.length()<1)return;
	m_fileprocessor->closeFile();
	if(m_fileprocessor->loadFile(filename)){

		m_detection->testEvents(m_fileprocessor->getEventPtrF(), m_fileprocessor->getTotalEvents());

		disconnect(m_fileprocessor, SIGNAL(eventsReadF(EventF*,int)), m_detection, SLOT(analyzeLiveEvents(EventF*,int)));
		disconnect(m_fileprocessor, SIGNAL(eventsReadF(EventF*,int)), m_detection, SLOT(testLiveEvents(EventF*,int)));
		connect(m_fileprocessor, SIGNAL(eventsReadF(EventF*,int)), m_detection, SLOT(testLiveEvents(EventF*,int)));
	}
	else{
		m_ui->statusBar->showMessage("\nUnable to open " + filename + "!");
	}
}

void EDVSD_Viewer::on_action_Pause_triggered()
{
	m_visualizer->setPaused(m_ui->action_Pause->isChecked());
}

void EDVSD_Viewer::uncheckAllReplaySpeed()
{
	m_ui->action1_1x->setChecked(false);
	m_ui->action1_2x->setChecked(false);
	m_ui->action1_4x->setChecked(false);
	m_ui->action1_8x->setChecked(false);
	m_ui->action1_16x->setChecked(false);
	m_ui->action1_32x->setChecked(false);
	m_ui->action1_64x->setChecked(false);
	m_ui->action1_128x->setChecked(false);
	m_ui->action1_256x->setChecked(false);
	m_ui->action1_512x->setChecked(false);
	m_ui->action1_1024x->setChecked(false);
	m_ui->action1_2048x->setChecked(false);
}

void EDVSD_Viewer::on_action1_1x_triggered()
{
	uncheckAllReplaySpeed();
	m_ui->action1_1x->setChecked(true);
	m_replay_speed = 33000 / 1; //33000 us per frame
	m_replay_speed_state = 0;
}

void EDVSD_Viewer::on_action1_2x_triggered()
{
	uncheckAllReplaySpeed();
	m_ui->action1_2x->setChecked(true);
	m_replay_speed = 33000 / 2;
	m_replay_speed_state = 1;
}

void EDVSD_Viewer::on_action1_4x_triggered()
{
	uncheckAllReplaySpeed();
	m_ui->action1_4x->setChecked(true);
	m_replay_speed = 33000 / 4;
	m_replay_speed_state = 2;
}

void EDVSD_Viewer::on_action1_8x_triggered()
{
	uncheckAllReplaySpeed();
	m_ui->action1_8x->setChecked(true);
	m_replay_speed = 33000 / 8;
	m_replay_speed_state = 3;
}

void EDVSD_Viewer::on_action1_16x_triggered()
{
	uncheckAllReplaySpeed();
	m_ui->action1_16x->setChecked(true);
	m_replay_speed = 33000 / 16;
	m_replay_speed_state = 4;
}

void EDVSD_Viewer::on_action1_32x_triggered()
{
	uncheckAllReplaySpeed();
	m_ui->action1_32x->setChecked(true);
	m_replay_speed = 33000 / 32;
	m_replay_speed_state = 5;
}

void EDVSD_Viewer::on_action1_64x_triggered()
{
	uncheckAllReplaySpeed();
	m_ui->action1_64x->setChecked(true);
	m_replay_speed = 33000 / 64;
	m_replay_speed_state = 6;
}

void EDVSD_Viewer::on_action1_128x_triggered()
{
	uncheckAllReplaySpeed();
	m_ui->action1_128x->setChecked(true);
	m_replay_speed = 33000 / 128;
	m_replay_speed_state = 7;
}

void EDVSD_Viewer::on_action1_256x_triggered()
{
	uncheckAllReplaySpeed();
	m_ui->action1_256x->setChecked(true);
	m_replay_speed = 33000 / 256;
	m_replay_speed_state = 8;
}

void EDVSD_Viewer::on_action1_512x_triggered()
{
	uncheckAllReplaySpeed();
	m_ui->action1_512x->setChecked(true);
	m_replay_speed = 33000 / 512;
	m_replay_speed_state = 9;
}

void EDVSD_Viewer::on_action1_1024x_triggered()
{
	uncheckAllReplaySpeed();
	m_ui->action1_1024x->setChecked(true);
	m_replay_speed = 33000 / 1024;
	m_replay_speed_state = 10;
}

void EDVSD_Viewer::on_action1_2048x_triggered()
{
	uncheckAllReplaySpeed();
	m_ui->action1_2048x->setChecked(true);
	m_replay_speed = 33000 / 2048;
	m_replay_speed_state = 11;
}

void EDVSD_Viewer::executeReplaySpeed()
{
	switch(m_replay_speed_state){
	case 0:
		on_action1_1x_triggered();
		break;
	case 1:
		on_action1_2x_triggered();
		break;
	case 2:
		on_action1_4x_triggered();
		break;
	case 3:
		on_action1_8x_triggered();
		break;
	case 4:
		on_action1_16x_triggered();
		break;
	case 5:
		on_action1_32x_triggered();
		break;
	case 6:
		on_action1_64x_triggered();
		break;
	case 7:
		on_action1_128x_triggered();
		break;
	case 8:
		on_action1_256x_triggered();
		break;
	case 9:
		on_action1_512x_triggered();
		break;
	case 10:
		on_action1_1024x_triggered();
		break;
	case 11:
		on_action1_2048x_triggered();
		break;
	}
}

void EDVSD_Viewer::on_actionSlower_triggered()
{
	m_replay_speed_state = min(11, m_replay_speed_state + 1);
	executeReplaySpeed();
}

void EDVSD_Viewer::on_actionFaster_triggered()
{
	m_replay_speed_state = max(0, m_replay_speed_state - 1);
	executeReplaySpeed();
}
