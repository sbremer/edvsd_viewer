#include "edvsd_viewer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    EDVSD_Viewer w;
	w.showMaximized();
    
    return a.exec();
}
