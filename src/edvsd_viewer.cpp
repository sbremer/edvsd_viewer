#include "edvsd_viewer.h"
#include "ui_edvsd_viewer.h"

EDVSD_Viewer::EDVSD_Viewer(QWidget *parent) :
    QMainWindow(parent),
	m_ui(new Ui::EDVSD_Viewer)
{
	m_ui->setupUi(this);
	m_fileprocessor = new EDVSD_FileProcessor();
	m_visualizer = NULL;
}

EDVSD_Viewer::~EDVSD_Viewer()
{
	delete m_ui;
	delete m_fileprocessor;
	if(m_visualizer!=NULL)
		delete m_visualizer;

}

void EDVSD_Viewer::on_actionE_xit_triggered()
{
	this->close();
}

void EDVSD_Viewer::on_action_Close_File_triggered()
{
	m_fileprocessor->closeFile();
}

void EDVSD_Viewer::on_action_Open_File_triggered()
{
	QString filename = QFileDialog::getOpenFileName(this, "Load dvsd file", "", "*.dvsd");
	if(m_fileprocessor->loadFile(filename)){

	}
	else{
		m_ui->statusBar->setStatusTip("Unable to open " + filename + "!");
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
