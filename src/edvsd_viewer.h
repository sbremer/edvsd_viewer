#ifndef EDVSD_VIEWER_H
#define EDVSD_VIEWER_H

#include <QMainWindow>
#include <QFileDialog>

#include "edvsd_visualizer.h"

namespace Ui {
class EDVSD_Viewer;
}

class EDVSD_Viewer : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit EDVSD_Viewer(QWidget *parent = 0);
    ~EDVSD_Viewer();
    
private slots:
	void on_actionE_xit_triggered();

	void on_action_Close_File_triggered();

	void on_action_Open_File_triggered();

	void on_action_Green_Red_triggered();

	void on_action_White_Black_triggered();

private:
	Ui::EDVSD_Viewer *m_ui;
	EDVSD_Visualizer *m_visualizer;


};

#endif // EDVSD_VIEWER_H
