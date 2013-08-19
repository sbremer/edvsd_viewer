#include "edvsd_viewer.h"
#include "ui_edvsd_viewer.h"

EDVSD_Viewer::EDVSD_Viewer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EDVSD_Viewer)
{
    ui->setupUi(this);
}

EDVSD_Viewer::~EDVSD_Viewer()
{
    delete ui;
}
