#ifndef EDVSD_VIEWER_H
#define EDVSD_VIEWER_H

#include <QMainWindow>
#include <QFileDialog>

#include "edvsd_visualizer.h"
#include "edvsd_fileprocessor.h"
#include "edvsd_anormaly_detection.h"

#include "genetricalgorithm_driver/geneticalgorithm_driver.h"

namespace Ui {
class EDVSD_Viewer;
}

class EDVSD_Viewer : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit EDVSD_Viewer(QWidget *parent = 0);
    ~EDVSD_Viewer();

public slots:
	void loadEventData();
    
private slots:
	void on_actionE_xit_triggered();

	void on_action_Close_File_triggered();

	void on_action_Open_File_triggered();

	void on_action_Green_Red_triggered();

	void on_action_White_Black_triggered();

	void on_actionDump_NNData_triggered();

	void on_action_GA_triggered();

	void on_action_Testopen_File_triggered();

	void on_action_Pause_triggered();

private:
	Ui::EDVSD_Viewer *m_ui;
	EDVSD_Visualizer *m_visualizer;
	EDVSD_FileProcessor *m_fileprocessor;
	EDVSD_Anormaly_Detection *m_detection;

	vector<double> m_tracking_param;

	vector<EventF> m_eventbuffer;

};

#endif // EDVSD_VIEWER_H
